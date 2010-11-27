/* palletViewer.cc

*/

#include <stdio.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <time.h>
#include <math.h>
#include "view.h"    // for draw_string, gl functions, glut functions
#include "xml_parser.h"
#include "packlist.h"
#include "response.h"
#include "asBuilt.h"
#include "palletViewer.h"

#define MINIMUM_OVERLAP .4 // the minimum pallet overlap to avoid an error

/********************************************************************/

// PalletViewer static variables

AsBuilt             PalletViewer::asBuilt;  // model of stack as built
std::map <int, col> PalletViewer::color;    // map from box id to a color
col *               PalletViewer::colors;   // array of available box colors
unsigned int        PalletViewer::count = 0;// number of boxes displayed
PackPallet          PalletViewer::cpallet;  // model of pallet in packlist file
PackList            PalletViewer::list;     // model of entire packlist file

/********************************************************************/

/* PalletViewer::insertBox

Returned Value: none

Called By: PalletViewer::redraw

This draws a box at the given position using a method that will work
in a display list. The display area is actually 1 unit wide (to work
well with the camera and openGL) while the area it represents is a
square 2000 millimeters on each side, so the scale factor is 2000.

*/

void PalletViewer::insertBox( /* ARGUMENTS                                  */
 col boxColor,                /* color of box                               */
 unsigned int length,         /* length of box in mm                        */
 unsigned int width,          /* width of box in mm                         */
 unsigned int height,         /* height of box in mm                        */
 int orient,                  /* 1 = length parallel to X                   */
 float posX,                  /* X position of center of box in mm          */
 float posY,                  /* Y position of center of box in mm          */
 float posZ,                  /* Z position of center of box in mm          */
 bool solid)                  /* true = faces and edges, false = edges only */
{
  static double minX;
  static double minY;
  static double minZ;
  static double maxX;
  static double maxY;
  static double maxZ;
  static double length2; // half of the length, scaled from mm to meters
  static double width2;  // half of the width,  scaled from mm to meters
  static double height2; // half of the height, scaled from mm to meters
  static GLubyte allIndices[] = {4,5,6,7, 1,2,6,5, 0,1,5,4,
				 0,3,2,1, 0,4,7,3, 2,3,7,6};
  static GLfloat vertices[] = {0,0,0, 0,0,0, 0,0,0, 0,0,0,
			       0,0,0, 0,0,0, 0,0,0, 0,0,0};
  
  glEnableClientState(GL_VERTEX_ARRAY);
  if(orient == 1)
    {
      length2 = ((float)length / 4000.0);
      width2 = ((float)width / 4000.0);
    }
  else
    {
      length2 = ((float)width / 4000.0);
      width2 = ((float)length / 4000.0);
    }
  height2 = ((float)height / 4000.0);
  posX /= 2000.0;
  posY /= 2000.0;
  posZ /= 2000.0;
  minX = (posX - length2);
  minY = (posY - width2);
  minZ = (posZ - height2);
  maxX = (posX + length2);
  maxY = (posY + width2);
  maxZ = (posZ + height2);
  vertices[0]  = (GLfloat)minX;
  vertices[1]  = (GLfloat)minY;
  vertices[2]  = (GLfloat)minZ;
  vertices[3]  = (GLfloat)minX;
  vertices[4]  = (GLfloat)maxY;
  vertices[5]  = (GLfloat)minZ;
  vertices[6]  = (GLfloat)minX;
  vertices[7]  = (GLfloat)maxY;
  vertices[8]  = (GLfloat)maxZ;
  vertices[9]  = (GLfloat)minX;
  vertices[10] = (GLfloat)minY;
  vertices[11] = (GLfloat)maxZ;
  vertices[12] = (GLfloat)maxX;
  vertices[13] = (GLfloat)minY;
  vertices[14] = (GLfloat)minZ;
  vertices[15] = (GLfloat)maxX;
  vertices[16] = (GLfloat)maxY;
  vertices[17] = (GLfloat)minZ;
  vertices[18] = (GLfloat)maxX;
  vertices[19] = (GLfloat)maxY;
  vertices[20] = (GLfloat)maxZ;
  vertices[21] = (GLfloat)maxX;
  vertices[22] = (GLfloat)minY;
  vertices[23] = (GLfloat)maxZ;
  glVertexPointer(3, GL_FLOAT, 0, vertices);
  glColor3f(boxColor.r, boxColor.g, boxColor.b);
  if (solid)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, allIndices);
      glColor3f(0.0f, 0.0f, 0.0f);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, allIndices);
    }
  else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, allIndices);
    }
}

/********************************************************************/

/* PalletViewer::init

Returned Value: none

Called By: main (in main.cc)

This initializes the palletViewer by reading the files, calculating
metrics, making the colors, and inserting the colors in the color map.

For all of the metrics, data for the stack up to and including package
N is calculated and stored in the package N data. That way, the data
is calculated only once and simply retrieved as the stack is loaded in
order or unloaded in reverse order.

*/

void PalletViewer::init( /* ARGUMENTS             */
 char * orderFile,       /* name of order file    */
 char * packlistFile,    /* name of packlist file */
 char * asBuiltFile)     /* name of as-built file */
{
  Order order;
  unsigned int i;

  order = read_order(orderFile, 0);
  list = read_response(packlistFile, 0);
  if (asBuiltFile[0])
    asBuilt = read_asBuilt(asBuiltFile, order, 1);
  cpallet = list.packpallet[0];
  cpallet.findOverlaps(MINIMUM_OVERLAP);
  cpallet.findCogs();
  cpallet.findIntersections();
  cpallet.findOverhangs();
  cpallet.findPressures();
  cpallet.findPressureMetrics();
  cpallet.findVolumes();
  
  makeColors(order.n_orderline());
  for (i = 0; i < order.n_orderline(); i++)
    {
      color.insert(std::pair<int, col>
		   (order.orderline[i].article.id, colors[i]));
    }
}

/********************************************************************/

/* palletViewer::makeColors

Returned Value: none

Called By: PalletViewer::init

This makes an array of howMany colors by picking points in the RGB cube.
The points gradually become dense in the cube. Colors that are dim
(i.e. the sum of the components is less than 0.4) are not used. Greyish
colors (near the diagonal from (0,0,0) to (1,1,1) are not used.
Colors that are adjacent in the array are generally far apart in the
cube. The first N colors generated for any value of howMany are the same
as the first N colors for any other value of howMany (as long as N is
smaller than both).

Each time around the outer loop, all points generated so far are used
to generate up to 6 new points by adding "jump" to the values of
R, G, B, RG, GB, and RB. Newly generated points outside the cube are
not used, and dim colors are not used. Adding jump to all of RGB is not
used in order to stay away from the central diagonal. At the end of
the outer loop, the jump size is cut in half.

The first color this makes is black in order to seed the outer loop.
In order uphold the "no dim colors" rule, this makes one extra color
and sets colors to point to the second element of the array.

Multiple inner loops are used rather than a single inner loop in order
to keep adjacent colors in the array far apart in the RGB cube.

*/

void PalletViewer::makeColors( /* ARGUMENTS             */
 int howMany)                  /* size of array to make */
{
  int n;      // total number of colors at end of last go-around
  float jump; // how far to move
  int m;      // index to fill in
  int i;      // index for 0 to n
  col * cols; // another handle on the array

  if (howMany < 1)
    { // quit if howMany is zero or negative
      fprintf(stderr, "Number of orders = %d, exiting\n", howMany);
      exit(1);
    }
  howMany++;
  cols = new col[howMany];
  colors = (cols + 1);
  cols[0].r = 0.0;
  cols[0].g = 0.0;
  cols[0].b = 0.0;
  n = 1;
  m = n;

  for (jump = 1.0; m < howMany; n = (m - 1), jump = (jump / 2))
    {
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].r + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + jump) > 0.4))
	    {
	      cols[m].r = (cols[i].r + jump);
	      cols[m].g = cols[i].g;
	      cols[m].b = cols[i].b;
	      if (++m == howMany)
		return;
	    }
	}
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].g + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + jump) > 0.4))
	    {
	      cols[m].r = cols[i].r;
	      cols[m].g = (cols[i].g + jump);
	      cols[m].b = cols[i].b;
	      if (++m == howMany)
		return;
	    }
	}
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].b + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + jump) > 0.4))
	    {
	      cols[m].r = cols[i].r;
	      cols[m].g = cols[i].g;
	      cols[m].b = (cols[i].b + jump);
	      if (++m == howMany)
		return;
	    }
	}
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].r + jump) <= 1.0) &&
	      ((cols[i].g + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + (2 * jump)) > 0.4))
	    {
	      cols[m].r = (cols[i].r + jump);
	      cols[m].g = (cols[i].g + jump);
	      cols[m].b = cols[i].b;
	      if (++m == howMany)
		return;
	    }
	}
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].g + jump) <= 1.0) &&
	      ((cols[i].b + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + (2 * jump)) > 0.4))
	    {
	      cols[m].r = cols[i].r;
	      cols[m].g = (cols[i].g + jump);
	      cols[m].b = (cols[i].b + jump);
	      if (++m == howMany)
		return;
	    }
	}
      for (i = 0; i < n; i++)
	{
	  if (((cols[i].r + jump) <= 1.0) &&
	      ((cols[i].b + jump) <= 1.0) &&
	      ((cols[i].r + cols[i].g + cols[i].b + (2 * jump)) > 0.4))
	    {
	      cols[m].r = (cols[i].r + jump);
	      cols[m].g = cols[i].g;
	      cols[m].b = (cols[i].b + jump);
	      if (++m == howMany)
		return;
	    }
	}
    } 
}

/********************************************************************/

/* PalletViewer::printText

Returned Value: none

Called By: displayMetricsWindow (in view.cc)

This prints the metrics that have been calculated by PalletViewer::init.
The text is printed in the metrics window. Its size and position do
not vary when the screen size is changed.

*/

#define str_length 200
void PalletViewer::printText( /* ARGUMENTS                 */
 int height)                  /* side of screen, in pixels */
{
  char str[str_length];                // string to print in
  Package * pack;                      // package to get data from
  std::list<int>::iterator iter;       // iterator for intersections list
  std::list<AreaData*>::iterator ator; // iterator for ups and downs lists
  int k;                               // counter for str
  int n;                               // counter for loading errors

  glColor3f(0.7f, 0.7f, 0.7f);
  snprintf(str, str_length, "CURRENT PACKAGE METRICS\n");
  drawString(20.0f, (float)(height - 20), GLUT_BITMAP_HELVETICA_10, str);
  if (count == cpallet.n_package())
    snprintf(str, str_length, "FINISHED STACK METRICS\n");
  else
    snprintf(str, str_length, "CURRENT STACK METRICS\n");
  drawString(20.0f, (float)(height - 145), GLUT_BITMAP_HELVETICA_10, str);

  if (count > 0)
    {
      pack = &(cpallet.package[count-1]);
      snprintf(str, str_length, "Package sequence number: #%d\n", count);
      drawString(20.0f, (float)(height - 35), GLUT_BITMAP_HELVETICA_10, str);

      k = snprintf(str, str_length, "Connected below to: %d", (int)pack->downs.size());
      if (pack->downs.size() > 0)
	{
	  for (ator = pack->downs.begin(); ator != pack->downs.end(); ator++)
		  k += snprintf(str + k, str_length - k, " #%d", (*ator)->pack->pack_sequence);
	}
      snprintf(str + k, str_length - k, "\n");
      drawString(20.0f, (float)(height - 50), GLUT_BITMAP_HELVETICA_10, str);

      k = snprintf(str, str_length, "Overlap fraction: %.4lf", pack->overlapFraction);
      if (pack->overlapFraction < MINIMUM_OVERLAP)
	k += snprintf(str + k, str_length - k, " error!");
      snprintf(str + k, str_length - k, "\n");
      drawString(20.0f, (float)(height - 65), GLUT_BITMAP_HELVETICA_10, str);

      snprintf(str, str_length, "Overhangs: X+%.1lf X-%.1lf Y+%.1lf Y-%.1lf mm\n",
	      pack->xPlusOver, pack->xMinusOver,
	      pack->yPlusOver, pack->yMinusOver);
      drawString(20.0f, (float)(height - 80), GLUT_BITMAP_HELVETICA_10, str);

      if (pack->ups.size() == 0)
	snprintf(str, str_length, "Maximum pressure on top: 0\n");
      else
	{
	  k = snprintf(str, str_length, "Maximum pressure on top: %.2lf Kg/m*m #%d",
		      pack->maxBoxPressure, pack->maxBoxSeqNo);
	  if ((pack->article.robustness.maxPressureOnTop >= 0.0) &&
	      (pack->maxBoxPressure >
	       (1000.0 * pack->article.robustness.maxPressureOnTop)))
	    k += snprintf(str + k, str_length - k, " error!");
	  snprintf(str + k, str_length - k, "\n");
	}
      drawString(20.0f, (float)(height - 95), GLUT_BITMAP_HELVETICA_10, str);

      k = snprintf(str, str_length, "Intersection errors: %d", (int)pack->inters.size());
      if (pack->inters.size() > 0)
	{
	  for (iter = pack->inters.begin(); iter != pack->inters.end(); ++iter)
	    {
	      if (*iter == 0)
		k += snprintf(str + k, str_length - k, " P");
	      else
		k += snprintf(str + k, str_length - k, " #%d", *iter);
	    }
	}
      snprintf(str + k, str_length - k, "\n");
      drawString(20.0f, (float)(height - 110), GLUT_BITMAP_HELVETICA_10, str);

      k = snprintf(str, str_length, "Loading order errors: %d", pack->loadingErrors);
      if (pack->loadingErrors > 0)
	{ // errors are at end of downs list
	  for (n = pack->downs.size(), ator = pack->downs.begin();
	       n > pack->loadingErrors; n--, ++ator); // skip the beginning
	  for ( ; ator != pack->downs.end(); ++ator)
	    k += snprintf(str + k, str_length - k, " #%d", (*ator)->pack->pack_sequence);
	}
      snprintf(str + k, str_length - k, "\n");
      drawString(20.0f, (float)(height - 125), GLUT_BITMAP_HELVETICA_10, str);

      snprintf(str, str_length, "Packages on stack: %d of %d\n", count, cpallet.n_package());
      drawString(20.0f, (float)(height - 160), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total weight: %.3f Kg\n", pack->stackWeight);
      drawString(20.0f, (float)(height - 175), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Stack height: %.3lf m\n", pack->stackHeight);
      drawString(20.0f, (float)(height - 190), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Volume of boxes: %.4lf cubic m\n", pack->stackBoxVolume);
      drawString(20.0f, (float)(height - 205), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Pallet storage volume: %.4lf cubic m\n",
	      pack->stackStorageVolume);
      drawString(20.0f, (float)(height - 220), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Volume density: %.4f\n", pack->stackDensity);
      drawString(20.0f, (float)(height - 235), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Pallet average overlap fraction: %.2f\n",
	      pack->stackOverlapFraction);
      drawString(20.0f, (float)(height - 250), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Pallet average connections below: %.2f\n",
	      pack->stackAverageConnects);
      drawString(20.0f, (float)(height - 265), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "COG height: %.4f m\n", pack->cogZ);
      drawString(20.0f, (float)(height - 280), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "COG relative offsets: X%+.4f Y%+.4f\n",
	      pack->cogRelX, pack->cogRelY);
      drawString(20.0f, (float)(height - 295), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Overhangs: X+%.1lf X-%.1lf Y+%.1lf Y-%.1lf mm\n",
	      pack->stackXPlusOver, pack->stackXMinusOver,
	      pack->stackYPlusOver, pack->stackYMinusOver);
      drawString(20.0f, (float)(height - 310), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total overlap errors: %d\n", pack->stackOverlapErrors);
      drawString(20.0f, (float)(height - 325), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total intersection errors: %d\n", pack->stackInters);
      drawString(20.0f, (float)(height - 340), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total loading order errors: %d\n",
	      pack->stackLoadingErrors);
      drawString(20.0f, (float)(height - 355), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total maximum pressure errors: %d\n",
	      pack->stackPressureErrors);
      drawString(20.0f, (float)(height - 370), GLUT_BITMAP_HELVETICA_10, str);
      snprintf(str, str_length, "Total errors: %d\n",
	      (pack->stackOverlapErrors + pack->stackInters +
	       pack->stackLoadingErrors + pack->stackPressureErrors));
      drawString(20.0f, (float)(height - 385), GLUT_BITMAP_HELVETICA_10, str);
    }
}

/***********************************************************************/

/* drawString

Returned Value: none

Called By: PalletViewer::printText

This prints one line of text starting at the given (x,y) location (in
currently active screen coordinates). The text is printed left to right
in the normal fashion for the English language.

The font argument is actually a symbol such as GLUT_BITMAP_HELVETICA_10.

*/

void PalletViewer::drawString( /* ARGUMENTS                   */
 float x,                      /* X location of start of text */
 float y,                      /* Y location of start of text */
 void * font,                  /* the font to use             */
 char* string)                 /* the text to print           */
{
  char * c;

  glRasterPos2f(x, y);
  for (c = string; *c != '\0'; c++)
    {
      glutBitmapCharacter(font, *c);
    }
}

/********************************************************************/

/* palletViewer::recalculate

Returned Value: none

Called By: keyboard (in view.cc) - change argument will be 1 or -1

This is called if the f or g key is pressed.

If count is greater than 0 and f is pressed, this subtracts 1 from count.
If count is less than the total number of packages and g is pressed,
this adds 1 to count.

*/

void PalletViewer::recalculate( /* ARGUMENTS */
 int change)                    /* 1, or -1  */
{
  if (((count > 0) && (change == -1)) ||
      ((count < cpallet.n_package()) && (change == 1)))
    count += change;
}

/********************************************************************/

/* PalletViewer::redraw

Returned Value: none

Called By: buildDisplayList (in view.cc)

This draws the pallet as planned in the packlist. If there is an asBuilt,
it also draws that.

The package numbers this uses are sequential starting with 1 (since
the PackSequence numbers start at 1, not 0). This is assuming the
PackSequence numbers are sequential. Otherwise, the package numbers
displayed will not match the numbers in the packlist.

The packlist pallet origin is in its lower left corner (when viewing
the XY plane in the usual orientation (+X right, +Y up) from above
(+Z).

The packlist pallet is offset into the third quadrant, so that the upper right
corner of the pallet is at the origin of the coordinate system.
The origin of the coordinate system is at the center of the grid.

The asBuilt pallet is offset into the first quadrant.

*/

void PalletViewer::redraw() /* NO ARGUMENTS   */
{
  float radToDeg = (float)57.29577958; // for asBuilt
  float offsetX;
  float offsetY;
  double asOffsetX; // for asBuilt
  double asOffsetY; // for asBuilt
  double asOffsetZ; // for asBuilt
  col boxColor;

  Point pos;
  Article * art;
  int orient;
  unsigned int i;

  // draw the pallet in the third quadrant with its top on the XY plane
  boxColor.r = 0.5;
  boxColor.g = 0.5;
  boxColor.b = 0.5;
  offsetX = -(float)(cpallet.dimension.length);
  offsetY = -(float)(cpallet.dimension.width);
  asOffsetX = 1.800;
  asOffsetY = -0.800;
  asOffsetZ = 2.100;
  
  insertBox(boxColor, cpallet.dimension.length, cpallet.dimension.width,
	    40, 1, (float)-(cpallet.dimension.length / 2.0),
	    (float)-(cpallet.dimension.width /2.0), (float)-20.0, true);
  for (i = 0; i < count; i++)
    {
      // draw box on pallet as planned
      pos = cpallet.package[i].place_position;
      art = &(cpallet.package[i].article);
      orient = cpallet.package[i].orientation;
      insertBox(color[art->id], art->length, art->width, art->height,
		orient, (offsetX + (float)pos.x), (offsetY + (float)pos.y),
		(float)(pos.z - art->height/2.0), true);
      // draw box on pallet as built
      if (i < asBuilt.packages.size())
	{
	  art = &(asBuilt.packages[i].article);
	  glPushMatrix();
	  glTranslated((0.2 * (asOffsetX + asBuilt.packages[i].x)),
		       (0.2 * (asOffsetY + asBuilt.packages[i].y)),
		       (0.2 * (asOffsetZ - asBuilt.packages[i].z)));
	  glRotated(asBuilt.packages[i].roll  * radToDeg, 1.0, 0.0, 0.0);
	  glRotated(asBuilt.packages[i].pitch * radToDeg, 0.0, 1.0, 0.0);
	  glRotated(asBuilt.packages[i].yaw   * radToDeg, 0.0, 0.0, 1.0);
	  insertBox(color[art->id], art->length, art->width, art->height, 1,
		    0.0, 0.0, 0.0, false);
	  glPopMatrix();
	}
    }
}

/********************************************************************/
