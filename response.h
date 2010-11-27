/*
 * Class structure to parse output pallet configuration for a mixed pallet.
 * KUKA output file definition for orders is used here.
 *
 * Author: Pushkar Kolhe
*/

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include "packlist.h"
#include <math.h>
#include <list>

class Dimensions;
class Point;
class Package;
class AreaData;
class PackList;
class PackPallet;

/********************************************************************/

/* Dimensions

This is minimal data for a pallet. For example, there is no height.
The length goes in the X-direction of the coordinate system of the pallet,
the width in the Y-direction.

It seems likely that the max_load_height should include the height of
the pallet. Need to check on that.

"PalletDimensions" would be a better name for this class.

*/

class Dimensions {
 public:
  Dimensions();
  ~Dimensions();
  int parse(std::string data);   

  unsigned int length;           // length in millimeters
  unsigned int width;            // width in millimeters
  unsigned int max_load_weight;  // maximum load in grams
  unsigned int max_load_height;  // maximum height of load in millimeters
};

/********************************************************************/

/* Point

A Point is defined by its X, Y, and Z coordinates, which are given
in millimeters in the coordinate system of the pallet. Z is constrained
to be non-negative, so only Points above the pallet can be represented.

*/

class Point {
 public:
  Point();
  ~Point();
  int parse(std::string data);

  int x;
  int y;
  unsigned int z;
};

/********************************************************************/

/* Package

This is really a whole set of data about a box (or something shaped
like a box). Two attributes (article and barcode) together identify
and describe the box. The rest of the attributes are parameters for
placing the box on a stack of boxes on a pallet and for calculating
metrics for the stack.

The first 12 attributes (through stack_height_before) are those given
as elements of the Package element in the XML schema
OffLineResponse.xsd (but the names are reformatted). The remaining
attributes are either metrics for the package or data specific to the
package that is used in calculating metrics. The four attributes whose
names end in OverMax are data for the stack of packages that includes
this package and all other packages with smaller pack_sequence
numbers. The first 12 attributes get parsed from the XML file. The
rest are calculated.

Many of the values are doubles rather than unsigned ints. This is because
if the length or width of a box is an odd number, half of that is not
an integer.

Point locations for approach points are in the coordinate system of
the pallet. The units are millimeters.

The place_position and the approach points are located in the middle
of the top of the box.

The approach points are to be used as intermediate points in moving
the box.

If the bottom of the box is on the pallet, then overlapFraction is the
fraction of the area of the bottom of the box that is in contact
with the pallet.  Otherwise, the overlapFraction is the fraction of the
area of the bottom of the box that is in contact with another box.

Boxes B1 and B2 are judged to be in contact at the top of B1 and
bottom of B2 if and only if (1) the Z-values of the top of B1 and the
bottom of B2 differ by less than 0.1 millimeter and (2) some part of
the bottom of B2 is above some part of the top of B1.

The following four attributes modeled here are required in the
OffLineResponse.xsd file but do not appear in any of the sample XML
files (the sample files do not conform). Currently, their values are
not used anywhere.
  incoming_sequence
  orderlineno
  parent_layer
  stack_height_before

*/

class Package {
 public:
  Package();
  ~Package();
  int parse(std::string data);        // parse from XML file
  
  void findBoundaries();              // finds min and max
  double findOverlapBox(Package * p); // finds overlap area of this with box p
  double findOverlapPallet(unsigned int length, unsigned int width);
                                      // finds overlap with pallet
  void findOverhangs(double length, double width,
		     double * xPlusMax, double * xMinusMax,
		     double * yPlusMax, double * yMinusMax);
                                      // finds overhangs with pallet
  void updateCog(double * momentX, double * momentY, double * momentZ,
		 double halfLength, double halfWidth, double * stackWeightIn,
		 double * stackHeightIn);
                                      // updates COG to include this box
                                     
  unsigned int pack_sequence;       // number of boxes placed earlier, plus 1
  unsigned int incoming_sequence;   // parsed if present, not used
  unsigned int orderlineno;         // parsed if present, not used
  unsigned int parent_layer;        // parsed if present, not used
  Article article;                  // description of box and contents
  Barcode barcode;                  // bar code for specific instance of article
  Point place_position;             // see above
  unsigned int orientation;         // 1 = length parallel to X else parallel Y
  Point approach_point_1;           // first approach point
  Point approach_point_2;           // second approach point
  Point approach_point_3;           // third approach point
  unsigned int stack_height_before; // parsed if present, not used
  
  double area;              // area of top or bottom of box, square mm
  std::list<AreaData*> ups; // area data for boxes resting on this box
  double cogRelX;           // rel X coord of COG of stack thru this box
  double cogRelY;           // rel Y coord of COG of stack thru this box
  double cogZ;              // Z coord of COG of stack thru this box, m
  std::list<AreaData*> downs; // area data for boxes this box is resting on
  std::list<int> inters;    // seq nos of boxes intersecting this box, pallet=0
  double loadedWeight;      // box weight plus weight of boxes resting on it, Kg
  int loadingErrors;        // loading errors for this box
  double maxBoxPressure;    // maximum pressure on top of box, Kg/m*m
  int maxBoxSeqNo;          // sequence number of box exerting max pressure
  double maxX;              // maximum X value on placed box, mm
  double maxY;              // maximum Y value on placed box, mm
  double maxZ;              // maximum Z value on placed box, mm
  double minX;              // minimum X value on placed box, mm
  double minY;              // minimum Y value on placed box, mm
  double minZ;              // minimum Z value on placed box, mm
  double overlapFraction;   // see above, dimensionless
  double stackAverageConnects; // stack average connections below thru this box
  double stackBoxVolume;    // total volume of stack  thru this box, cubic m
  double stackDensity;      // stackBoxVolume/stackStorageVolume, dimensionless
  double stackHeight;       // height of stack thru this box, m
  int stackInters;          // total stack intersections thru this box
  int stackLoadingErrors;   // total stack loading errors thru this box
  int stackOverlapErrors;   // total stack overlap errors thru this box
  double stackOverlapFraction; // stack average overlap fraction thru this box
  int stackPressureErrors;  // total stack pressure errors thru this box
  double stackStorageVolume;// stackHeight times pallet area, cubic m
  double stackWeight;       // weight of stack thru this box, Kg
  double stackXMinusOver;   // stack -X overhang of pallet thru this box, mm
  double stackXPlusOver;    // stack +X overhang of pallet thru this box, mm
  double stackYMinusOver;   // stack -Y overhang of pallet thru this box, mm
  double stackYPlusOver;    // stack +Y overhang of pallet thru this box, mm
  double xMinusOver;        // box overhang of pallet in -X direction, mm
  double xPlusOver;         // box overhang of pallet in +X direction, mm
  double yMinusOver;        // box overhang of pallet in -Y direction, mm
  double yPlusOver;         // box overhang of pallet in +Y direction, mm  
};

/********************************************************************/

/* AreaData

An AreaData is triple consisting of a pointer to a Package, an area in
square meters, and a pressure in Kg per square meter. 

*/

class AreaData {
 public:
  AreaData();
  AreaData(Package * packIn, double areaIn);
  ~AreaData();

  Package * pack;
  double area;
  double pressure;
};

/********************************************************************/

class PackList {
 public:
  PackList();
  ~PackList();
  unsigned int n_packpallet();
  int parse(std::string data);

  unsigned int order_id;
  std::vector<PackPallet> packpallet;
};

/********************************************************************/

/* PackPallet

findIntersections finds intersections of each box with the pallet and
with boxes previously put on the stack.

*/

class PackPallet {
 public:
  PackPallet();
  ~PackPallet();
  void findCogs();
  void findIntersections();
  void findOverhangs();
  void findOverlaps(double minOverlapForError);
  void findPressures();
  void findPressureMetrics();
  void findVolumes();
  unsigned int n_package();
  int parse(std::string data);

  unsigned int pallet_number;
  unsigned int brutto_weight;
  unsigned int number_of_packages;
  std::string description;
  Dimensions dimension;
  std::vector<Package> package;
};

/********************************************************************/

PackList read_response(const char* filename, int debug);

#endif /* RESPONSE_H_ */
