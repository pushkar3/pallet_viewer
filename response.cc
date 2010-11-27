/* response.cc

*/

#include "packlist.h"
#include "xml_parser.h"
#include "response.h"
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <fstream>

typedef unsigned int uint;

/********************************************************************/

AreaData::AreaData() {};

/********************************************************************/

/* AreaData::AreaData

This constructor initializes:
 - pack to packIn
 - area to areaIn
 - pressure to -1.0 to indicate that it is not yet set.

*/

AreaData::AreaData(Package * packIn, double areaIn)
{
  pack = packIn;
  area = areaIn;
  pressure = -1.0;
}

/********************************************************************/

AreaData::~AreaData() {};

/********************************************************************/

Dimensions::Dimensions() {}

/********************************************************************/

Dimensions::~Dimensions() {}

/********************************************************************/

int Dimensions::parse(std::string data)
{
  length = atoi(xml_parse_tag(data, "Length").c_str());
  width = atoi(xml_parse_tag(data, "Width").c_str());
  max_load_height = atoi(xml_parse_tag(data, "MaxLoadHeight").c_str());
  max_load_weight = atoi(xml_parse_tag(data, "MaxLoadWeight").c_str());
  return strlen(data.c_str());
}

/********************************************************************/

Package::Package() {};

/********************************************************************/

Package::~Package() {};

/********************************************************************/

/* Package::findBoundaries

Called By: Package::parse

This sets minX, minY, and minZ of a Package to the point on the box
with minimum x, y, and z.  It sets maxX, maxY, and maxZ to the point
on the box with maximum x, y, and z.  The place position is in the
middle of the top of the box.

This also sets the area of the Package to the area of the top of the box.

This assumes the box is upright with sides parallel to the X and Y
axes (which is how all packages are assumed to placed).

*/

void Package::findBoundaries() /* NO ARGUMENTS */
{
  double xDist;
  double yDist;

  if(orientation == 1)
    { // length parallel to X axis
      xDist = (double)article.length;
      yDist = (double)article.width;
    }
  else
    { // length parallel to Y axis
      xDist = (double)article.width;
      yDist = (double)article.length;
    }
  area = xDist * yDist;
  minX = (double)place_position.x - (xDist / 2.0);
  maxX = (double)place_position.x + (xDist / 2.0);
  minY = (double)place_position.y - (yDist / 2.0);
  maxY = (double)place_position.y + (yDist / 2.0);
  minZ = (double)(place_position.z - article.height);
  maxZ = (double)place_position.z;
}

/********************************************************************/

/* Package::findOverhangs

Returned Value: none

Called By: PackPallet::findOverhangs

This calculates the four overhangs for this box (xPlusOver, etc.) and
the four maximum overhangs for the stack on the pallet up to and
including this box.  If the overhang for this box in any direction is
greater than the current stack overhang, the current stack overhang is
reset.

Before this is called, minX, minY, maxX, and maxY must already have been
set; findBoundaries does that.

*/

void Package::findOverhangs( /* ARGUMENTS                                    */
 double length,              /* length of X side of pallet                   */
 double width,               /* length of Y side of pallet                   */
 double * xPlusMax,          /* previous max +X overhang, maybe updated here */
 double * xMinusMax,         /* previous max -X overhang, maybe updated here */
 double * yPlusMax,          /* previous max +Y overhang, maybe updated here */
 double * yMinusMax)         /* previous max -Y overhang, maybe updated here */
{
  xPlusOver = ((maxX > length) ? (maxX - length) : 0.0);
  yPlusOver = ((maxY > width)  ? (maxY -  width) : 0.0);
  xMinusOver = ((minX < 0.0) ? -minX : 0.0);
  yMinusOver = ((minY < 0.0) ? -minY : 0.0);
  if (xPlusOver > *xPlusMax)
    *xPlusMax = xPlusOver;
  if (yPlusOver > *yPlusMax)
    *yPlusMax = yPlusOver;
  if (xMinusOver > *xMinusMax)
    *xMinusMax = xMinusOver;
  if (yMinusOver > *yMinusMax)
    *yMinusMax = yMinusOver;
  stackXPlusOver = *xPlusMax;
  stackYPlusOver = *yPlusMax;
  stackXMinusOver = *xMinusMax;
  stackYMinusOver = *yMinusMax;
}

/********************************************************************/

/* Package::findOverlapBox

Returned Value: double (the area of the part of the bottom of the box
in this package that is over the box in package p, in square millimeters)

Called By: PackPallet::findOverlaps

This should be called only if the bottom of the box in this package is
at the same height as the top of the box in package p. The function is
not checking that.

The part of the box in this package in contact with the box in package
p is a rectangle if there is any contact. This finds the length and
width of that rectangle.

*/

double Package::findOverlapBox( /* ARGUMENTS                         */
 Package * p)                   /* a package that might support this */
{
  double xDist;
  double yDist;

  if(orientation == 1)
    { // length parallel to X axis
      xDist = (double)article.length;
      yDist = (double)article.width;
    }
  else
    { // length parallel to Y axis
      xDist = (double)article.width;
      yDist = (double)article.length;
    }
  if ((minX >= p->maxX) || (maxX < p->minX) ||
      (minY >= p->maxY) || (maxY < p->minY))
    return 0;
  if (minX < p->minX)
    xDist = (xDist - (p->minX - minX));
  if (minY < p->minY)
    yDist = (yDist - (p->minY - minY));
  if (maxX > p->maxX)
    xDist = (xDist - (maxX - p->maxX));
  if (maxY > p->maxY)
    yDist = (yDist - (maxY - p->maxY));
  return (xDist * yDist);
}

/********************************************************************/

/* Package::findOverlapPallet

Returned Value: double (the area of the part of the bottom of the box that
is over the pallet, in square millimeters)

Called By: PackPallet::findOverlaps

This should be called only if minZ is zero, so that the box is resting
on the pallet. The function is not checking that.

The part of the box on the pallet is a rectangle if any part of the
box is on the pallet. This finds the length and width of that rectangle.

*/

double Package::findOverlapPallet( /* ARGUMENTS                       */
 unsigned int length,              /* length of pallet in millimeters */
 unsigned int width)               /* width of pallet in millimeters  */
{
  double xDist; // extent in X direction of the part of the box on the pallet
  double yDist;

  if(orientation == 1)
    { // length parallel to X axis
      xDist = (double)article.length;
      yDist = (double)article.width;
    }
  else
    { // length parallel to Y axis
      xDist = (double)article.width;
      yDist = (double)article.length;
    }
  if ((minX >= length) || (maxX < 0) ||
      (minY >= width)  || (maxY < 0))
    return 0;
  if (minX < 0)
    xDist = (xDist + minX);
  if (minY < 0)
    yDist = (yDist + minY);
  if (maxX > length)
    xDist = (xDist - (maxX - length));
  if (maxY > width)
    yDist = (yDist - (maxY - width));
  return (xDist * yDist);
}

/********************************************************************/

int Package::parse(std::string data)
{
  pack_sequence = atoi(xml_parse_tag(data, "PackSequence").c_str());
  incoming_sequence = atoi(xml_parse_tag(data, "IncomingSequence").c_str());
  orderlineno = atoi(xml_parse_tag(data, "OrderLineNo").c_str());
  parent_layer = atoi(xml_parse_tag(data, "ParentLayer").c_str());
  article.parse(xml_parse_tag(data, "Article"));
  barcode.parse(xml_parse_tag(data, "Barcode"));
  place_position.parse(xml_parse_tag(data, "PlacePosition"));
  orientation = atoi(xml_parse_tag(data, "Orientation").c_str());
  approach_point_1.parse(xml_parse_tag(data, "ApproachPoint1"));
  approach_point_2.parse(xml_parse_tag(data, "ApproachPoint2"));
  approach_point_3.parse(xml_parse_tag(data, "ApproachPoint3"));
  stack_height_before = atoi(xml_parse_tag(data, "StackHeightBefore").c_str());
  findBoundaries();
  return strlen(data.c_str());
}

/********************************************************************/

/* Package::updateCog

Returned Value: none

Called By: PackPallet::findCogs

This finds cogRelX, cogRelY, and cogZ for the Package. It also finds
stackWeight and stackHeight for the Package.

This also updates momentX, momentY, and momentZ.

The place position is in the middle of the top of the box. We are
assuming the COG of a box is at its center, so the Z value of the
COG of the box is half the height below the place position.

*/

void Package::updateCog( /* ARGUMENTS                                         */
 double * momentX,       /* total X moment around pallet center, updated here */
 double * momentY,       /* total Y moment around pallet center, updated here */
 double * momentZ,       /* total Z moment around pallet center, updated here */
 double halfLength,      /* half the length of the pallet                     */
 double halfWidth,       /* half the width of the pallet                      */
 double * stackWeightIn, /* total weight of stack, updated here               */
 double * stackHeightIn) /* total height of stack, maybe updated here         */
{
  double weight;
  double height;

  height = (place_position.z / 1000.0);
  if (height > *stackHeightIn)
    *stackHeightIn = height;
  stackHeight = *stackHeightIn;
  height -= (article.height / 2000.0);
  weight = (article.weight / 1000.0);
  *momentX += (((place_position.x / 1000.0) - halfLength) * weight);
  *momentY += (((place_position.y / 1000.0) - halfWidth) * weight);
  *momentZ += (height * weight);
  *stackWeightIn += weight;
  stackWeight = *stackWeightIn;
  cogRelX = (*momentX / (stackWeight * halfLength));
  cogRelY = (*momentY / (stackWeight * halfWidth));
  cogZ = (*momentZ / stackWeight);
}

/********************************************************************/

PackList::PackList() {}

/********************************************************************/

PackList::~PackList() {}

/********************************************************************/

unsigned int PackList::n_packpallet()
{
  return packpallet.size();
}

/********************************************************************/

int PackList::parse(std::string data)
{
  order_id = atoi(xml_parse_tag(data, "OrderID").c_str());
  int ret = 1;
  while(ret)
    {
      std::string s_packlist = xml_parse_tag(data, "PackPallet");
      if(strlen(s_packlist.c_str()) > 0) {
	PackPallet p;
	p.parse(s_packlist);
	packpallet.push_back(p);
      }
      ret = xml_parse_remove_first_tag(&data, "PackPallet");
    }
  return strlen(data.c_str());
}

/********************************************************************/

PackList read_response(const char* filename, int debug)
{
  std::ifstream ifs(filename);
  if(!ifs.is_open())
    {
      printf("%s File not found.\n Exiting.\n", filename);
      exit(1);
    }
  
  int buf_len = xml_parser_get_buffer_length(filename);
  char* packlist_buf = (char*) malloc (buf_len + 1);
  ifs.read(packlist_buf, buf_len);
  std::string packlist_xml = packlist_buf;
  
  PackList list;
  list.parse(xml_parse_tag(packlist_xml, "PackList"));
  
  if(debug)
    {
      printf("Number of PackPallets: %d\n", list.n_packpallet());
      
      for(uint i = 0; i < list.n_packpallet(); i++)
	{
	  printf("%d. Number of Packages: %d\n",
		 list.n_packpallet(), list.packpallet[i].n_package());
	  for(uint j = 0; j < list.packpallet[i].n_package(); j++)
	    {
	      printf("Pack Sequence: %d\t",
		     list.packpallet[i].package[j].pack_sequence);
	      printf("ID: %d \t Size: <%d, %d, %d>\n",
		     list.packpallet[i].package[j].article.id,
		     list.packpallet[i].package[j].article.width,
		     list.packpallet[i].package[j].article.height,
		     list.packpallet[i].package[j].article.length);
	    }
	}
    }
  
  printf("PackList Read.\n");
  free(packlist_buf);
  return list;
}

/********************************************************************/

PackPallet::PackPallet() {}

/********************************************************************/

PackPallet::~PackPallet() {}

/********************************************************************/

/* PackPallet::findCogs

Returned Value: none

Called By: PalletViewer::init

This calls updateCog for each package so that each package
has the center of gravity (COG) for the stack including all
packages up to and including that package.

The calls to updateCog also update the stackWeight and stackHeight of
each package.

The relative coordinates represent the fraction of half the length
or width of the pallet that the COG is off center. When the COG
is at the center, the relative coordinate is 0.0. When the COG is
at the edge of the pallet, the relative coordinate is 1.0 or -1.0.

*/

void PackPallet::findCogs() /* NO ARGUMENTS */
{
  double momentX = 0;     // total X moment around the center, Kg*m
  double momentY = 0;     // total Y moment around the center, Kg*m
  double momentZ = 0;     // total Z moment around the center, Kg*m
  unsigned int i;         // index for packages
  double halfLength;      // half the length of the pallet, m
  double halfWidth;       // half the width of the pallet, m
  double stackWeight = 0; // weight of stack, Kg
  double stackHeight = 0; // height of stack, m

  halfLength = (dimension.length / 2000.0);
  halfWidth =  (dimension.width / 2000.0);
  for (i = 0; i < n_package(); i++)
    {
      package[i].updateCog(&momentX, &momentY, &momentZ, halfLength,
			   halfWidth, &stackWeight, &stackHeight);
    }
}

/********************************************************************/

/* PackPallet::findIntersections

Returned Value: none

Called By: PalletViewer::init

This finds the intersections of each box with the pallet and
with boxes previously put on the stack. The sequence numbers of the
intersecting boxes are added to the inters list of a box being tested.
If a box intersects the pallet, 0 is put on the inters list.

This also sets the stackInters of each box.

In order for two boxes not to intersect, in at least one of the
three XYZ directions, the minimum of one box must be greater than
the maximum of the other.

*/

void PackPallet::findIntersections() /* NO ARGUMENTS */
{
  unsigned int i;      // counter for packages stacked on a pallet
  unsigned int j;      // counter for previously placed packages
  Package * pack;      // package being tested
  int totalInters = 0; // total number of intersections found

  for (i = 0; i < n_package(); i++)
    {
      pack = &(package[i]);
      if (pack->minZ < 0)
	pack->inters.push_back(0);
      for (j = 0; j < i; j++)
	{
	  if ((package[j].minX >= pack->maxX) ||
	      (package[j].maxX <= pack->minX) ||
	      (package[j].minY >= pack->maxY) ||
	      (package[j].maxY <= pack->minY) ||
	      (package[j].minZ >= pack->maxZ) ||
	      (package[j].maxZ <= pack->minZ));
	  else
	    pack->inters.push_back((int)(package[j].pack_sequence));
	}
      totalInters += pack->inters.size();
      pack->stackInters = totalInters;
    }
}

/********************************************************************/

/* PackPallet::findOverhangs

Returned Value: none

Called By: PalletViewer::init

This finds the pallet overhangs for each package in the +X, -X, +Y, and
-Y directions and records them in the data for the package.

*/

void PackPallet::findOverhangs() /* NO ARGUMENTS */
{
  double length;
  double width;
  double xPlusMax = 0;
  double xMinusMax = 0;
  double yPlusMax = 0;
  double yMinusMax = 0;
  unsigned int i;

  length = (double)dimension.length;
  width =  (double)dimension.width;
  for (i = 0; i < n_package(); i++)
    {
      package[i].findOverhangs(length, width,
			       &xPlusMax, &xMinusMax, &yPlusMax, &yMinusMax);
    }
}

/********************************************************************/

/* PackPallet::findOverlaps

Returned Value: none

Called By: PalletViewer::init

This goes through all the packages on the pallet. For each package,
packI, it does the following:

A. If packI is on the pallet, it calls findOverlapPallet.

B. Otherwise, it goes through all the packages and calls findOverlapBox
for each one, packJ, whose top is at the same level as the bottom of
packI. If packI sits on top of packJ:
 - A pointer to an AreaData is pushed onto the ups list of packJ.
   The AreaData consists of packI, areaJ, and an unset pressure (-1.0).
 - A pointer to an AreaData is pushed onto the downs list of packI.
   The AreaData consists of packJ, areaJ, and an unset pressure (-1.0).
 - The contact area (areaJ) is added to the total bottom contact area
   of packI.
 - If packJ is loaded after packI, 1 is added to the loadingErrors of
   packI and to the totalLoadingErrors.

C. Then it:
 - sets stackLoadingErrors of packI to the current totalLoadingErrors.
 - sets the overlapFraction of packI.
 - if the overlapFraction of packI is less than minOverlapForError,
   adds 1 to totalOverlapErrors.
 - sets the stackOverlapErrors of packI to the current totalOverlapErrors.
 - adds the overlap fraction of packI to the totalOverlap.
 - sets the stackOverlapFraction of packI.
 - sets the stackAverageConnects of packI.

Since distances and coordinates are integers, this is using an equality
test to determine if the bottom of one box is at the same height at the
top of another box.

When this is finished running, the ups and downs lists of each package
are complete, but all pressures in the elements of those lists are set
to -1.0

The overlap fraction of a box is the fraction of the area of the bottom
of the box that is in contact with the pallet or with other boxes.

The stackOverlapFraction includes (1) the boxes that are directly on
the pallet, for which overlap with the pallet is calculated and (2)
the boxes that are on other boxes, for which the overlap with other
boxes is calculated.

The stackAverageConnects includes (1) the boxes that are directly on
the pallet, for which the number of connections is 1, and (2) the
boxes that are on other boxes, for which the number of such other
boxes is counted.

The way the computation is being done, the first layer of a stack always
has a stackAverageConnects value of 1, and the more layers there are, the
larger the the stackAverageConnects tends to get.

An alternate method of calculating stackAverageConnects might be to
disregard boxes directly on the pallet. This would eliminate the effect
described in the preceding paragraph.

*/

void PackPallet::findOverlaps( /* ARGUMENTS                            */
 double minOverlapForError)    /* minimum overlap that is not an error */
{
  unsigned int i;             // counter 1 for packages stacked on a pallet
  unsigned int j;             // counter 2 for packages stacked on a pallet
  double areaJ;               // area of package[j] touching package[i]
  double areaI;               // total area of package[i] touching pallet or box
  Package * packI;            // pointer to package[i]
  Package * packJ;            // pointer to package[j]
  int totalOverlapErrors = 0; // total number of overlap errors in file.
  int totalLoadingErrors = 0; // total number of loading order errors in file.
  double totalOverlap = 0.0;  // sum of overlap fractions
  int totalConnections = 0;   // total connections
  
  for (i = 0; i < n_package(); i++)
    {
      packI = &(package[i]);
      packI->loadingErrors = 0;
      if (packI->minZ == 0)
	areaI = packI->findOverlapPallet(dimension.length, dimension.width);
      else
	{
	  areaI = 0;
	  for (j = 0; j < n_package(); j++)
	    {
	      packJ = &(package[j]);
	      if (packI->minZ == packJ->maxZ)
		{
		  areaJ = packI->findOverlapBox(packJ);
		  if (areaJ > 0.0)
		    {
		      packI->downs.push_back(new AreaData(packJ, areaJ));
		      packJ->ups.push_back(new AreaData(packI, areaJ));
		      areaI += areaJ;
		      if (j > i)
			{
			  (packI->loadingErrors)++;
			  totalLoadingErrors++;
			}
		    }
		}
	    }
	}
      packI->stackLoadingErrors = totalLoadingErrors;
      packI->overlapFraction = (areaI / packI->area);
      if (packI->overlapFraction < minOverlapForError)
	totalOverlapErrors++;
      packI->stackOverlapErrors = totalOverlapErrors;
      totalConnections += ((packI->minZ == 0.0) ? 1 : packI->downs.size());
      totalOverlap += packI->overlapFraction;
      packI->stackOverlapFraction = (double)(totalOverlap / (i+1));
      packI->stackAverageConnects = (double)(totalConnections / (i+1));
    }
}
  
/********************************************************************/

/* PackPallet::findPressureMetrics

Returned Value: none

Called By: PalletViewer::init

This finds the maximum pressure exerted on each box and the sequence
number of the box on top that exerts that pressure. It also records
the total number of pressure errors for the stack through the box.

The units for maxPressureOnTop in robustness are g/mm*mm, while the
other units are Kg/m*m, so a conversion is done for maxPressureOnTop.

*/

void PackPallet::findPressureMetrics()
{
  int totalPressureErrors = 0;         // total pressure errors
  Package * pack;                      // pointer to package[i]
  std::list<AreaData*>::iterator iter; // iterator for ups list
  unsigned int i;                      // counter for packages on a pallet

  for (i = 0; i < n_package(); i++)
    {
      pack = &(package[i]);
      pack->maxBoxPressure = 0.0;
      for (iter = pack->ups.begin(); iter != pack->ups.end(); ++iter)
	{
	  if ((*iter)->pressure > pack->maxBoxPressure)
	    {
	      pack->maxBoxPressure = (*iter)->pressure;
	      pack->maxBoxSeqNo = (*iter)->pack->pack_sequence;
	    }
	}
      if ((pack->article.robustness.maxPressureOnTop >= 0.0) &&
	  (pack->maxBoxPressure >
	   (1000.0 * pack->article.robustness.maxPressureOnTop)))
	totalPressureErrors++;
      pack->stackPressureErrors = totalPressureErrors;
    }
}

/********************************************************************/

/* PackPallet::findPressures

Returned Value: none

Called By: PalletViewer::init

This finds the loadedWeight of each package (which is its weight plus
the force exerted by packages on top). It also finds the ups and downs
of each package. Ups and downs are both AreaDatas, which consist of
(i) a pointer to a package, (ii) the area of the overlap with that package,
and (iii) the pressure for that area.

In the inner loop, a package is processed if (i) it has not already
been processed and (ii) it has no ups or all of its ups have pressures
greater than 0. A package has already been processed if and only if
its loaded weight is greater than 0.

The area is in square millimeters, so it needs to be converted to square
meters for finding pressure in Kg/m*m.

*/

void PackPallet::findPressures() /* NO ARGUMENTS */
{
  bool progress = true; // indicates whether inner loop made progress
  int processed = 0;    // number of packages processed.
  unsigned int i;       // counter for packages stacked on a pallet
  Package * packI;      // pointer to package[i]
  Package * packJ;      // pointer to another package
  std::list<AreaData*>::iterator iter; // list iterator for packI
  std::list<AreaData*>::iterator ator; // list iterator for packJ
  double totalArea;     // total area of downs of package[i] in square meters

  for (i = 0; i < n_package(); i++) // initialize loadedWeight for all packages
    package[i].loadedWeight = -1.0;
  while (progress)
    {
      progress = false;
      for (i = 0; i < n_package(); i++)
	{
	  packI = &(package[i]);
	  if (packI->loadedWeight > 0)
	    continue;
	  for (iter = packI->ups.begin(); iter != packI->ups.end(); ++iter)
	    if ((*iter)->pressure < 0)
	      break;
	  if (iter != packI->ups.end())
	    continue;
	  progress = true;
	  processed++;
	  totalArea = 0;
	  packI->loadedWeight = (packI->article.weight / 1000.0);
	  for (iter = packI->ups.begin(); iter != packI->ups.end(); ++iter)
	    {
	      packI->loadedWeight +=
		(((*iter)->pressure * (*iter)->area) / 1000000.0);
	    }
	  for (iter = packI->downs.begin(); iter != packI->downs.end(); ++iter)
	    {
	      totalArea += ((*iter)->area / 1000000.0);
	    }
	  for (iter = packI->downs.begin(); iter != packI->downs.end(); ++iter)
	    {
	      (*iter)->pressure = (packI->loadedWeight / totalArea);
	      packJ = (*iter)->pack;
	      for (ator = packJ->ups.begin(); ator != packJ->ups.end(); ++ator)
		{
		  if ((*ator)->pack == packI)
		    {
		      (*ator)->pressure = (*iter)->pressure;
		      break;
		    }
		}
	    }
	}
    }
  if (processed != (int)n_package())
    {
      fprintf(stderr, "Error in findPressures\n");
    }
}

/********************************************************************/

/* PackPallet::findVolumes

Returned Value: none

Called By: PalletViewer::init

This finds and sets the stackStorageVolume, stackBoxVolume, and
stackDensity for each box on the pallet.

The stackStorageVolume is the area of the pallet times the height
of the stack (not including the pallet). This represents the volume
needed to store the pallet in a warehouse (assuming none of the boxes
hang over the edge of the pallet), minus the volume of the pallet
itself.

*/

void PackPallet::findVolumes() /* NO ARGUMENTS */
{
  double palletArea;         // area of pallet in square meters
  double totalBoxVolume = 0; // total volume of boxes on stack in cubic meters
  Article * art;             // Article in pack
  unsigned int i;            // counter for packages stacked on a pallet
  Package * pack;            // pointer to package[i]

  palletArea =
    ((dimension.length * dimension.width) / 1000000.0);
  for (i = 0; i < n_package(); i++)
    {
      pack = &(package[i]);
      pack->stackStorageVolume = (palletArea * pack->stackHeight);
      art = &(pack->article);
      totalBoxVolume += ((art->height / 1000.0) *
			 (art->width / 1000.0) * (art->length / 1000.0));
      pack->stackBoxVolume = totalBoxVolume;
      pack->stackDensity = (pack->stackBoxVolume / pack->stackStorageVolume);
    }
}

/********************************************************************/

unsigned int PackPallet::n_package()
{
  return package.size();
}

/********************************************************************/

int PackPallet::parse(std::string data)
{
  pallet_number = atoi(xml_parse_tag(data, "PalletNumber").c_str());
  brutto_weight = atoi(xml_parse_tag(data, "BruttoWeight").c_str());
  number_of_packages = atoi(xml_parse_tag(data, "NumberOfPackages").c_str());
  description = xml_parse_tag(data, "Description");
  dimension.parse(xml_parse_tag(data, "Dimensions"));
  int ret = 1;
  while(ret)
    {
      std::string s_package = xml_parse_tag(data, "Package");
      if(strlen(s_package.c_str()) > 0) {
	Package p;
	p.parse(s_package);
	package.push_back(p);
      }
      ret = xml_parse_remove_first_tag(&data, "Package");
    }
  return strlen(data.c_str());
}

/********************************************************************/

Point::Point() {}

/********************************************************************/

Point::~Point() {}

/********************************************************************/

int Point::parse(std::string data)
{
  x = atoi(xml_parse_tag(data, "X").c_str());
  y = atoi(xml_parse_tag(data, "Y").c_str());
  z = atoi(xml_parse_tag(data, "Z").c_str());
  return strlen(data.c_str());
}

/********************************************************************/
