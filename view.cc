/* view.cc

This includes the glInit function and the functions that are called
by callbacks in the glMainLoop.

All motions of the display are performed using the mouse.

No time delays are used because they are not needed. Unless the user is
interacting with the display using the keyboard or mouse, this just sits
there and uses no processing time.

*/

#include <stdlib.h>          // exit
#include <stdio.h>           // printf, etc.
#include "view.h"            // glInit
#include "palletViewer.h" // PalletViewer::recalculate, PalletViewer::redraw
#include "mouse.hh"          

#define IMAGE_SIZE 4000000

static void buildDisplayList(void);
static void displayMetricsWindow(void);
static void displayPictureWindow(void);
static void flipImage(char * image, int width, int height);
void glInit(int argc, char* argv[], const char * winName);
static void keyboard(unsigned char key, int x, int y);
static void makeGrid(void);
static void motion(int x, int y);
static void mouse(int button, int state, int x, int y);
static void reshapePictureWindow(int width, int height);
static void reshapeMetricsWindow(int width, int height);
static int windowDump(void);

/********************************************************************/

static int dump = 0;            // whether to dump image
static GLuint stackList = 0;    // display list for grid, pallet, and boxes
static int pictureExtent = 600; // width and height of picture window
static int metricsHeight = 405; // height of metrics window
static int metricsWidth = 260;  // width of metrics window
static int metricsWindow = 0;   // id of metrics window
static int pictureWindow = 0;   // id of picture window

/********************************************************************/

/* buildDisplayList

Returned Value: none

Called By:
  glInit
  keyboard

This makes the display list named stackList that includes the background
grid, the pallet, the stack of boxes as planned, and the asBuilt stack of
boxes.

*/

static void buildDisplayList(void) /* NO ARGUMENTS */
{
  // destroy any previous lists
  if (stackList)
    glDeleteLists(stackList, 1);
  // generate a display list
  stackList = glGenLists(1);
  glNewList(stackList, GL_COMPILE);
  makeGrid();
  PalletViewer::redraw();
  glEndList();
}
  
/***********************************************************************/

/* displayMetricsWindow

Returned Value: none

Called By: glInit (as glutDisplayFunc for metricsWindow)

This draws the metricsWindow.

*/

static void displayMetricsWindow(void) /* NO ARGUMENTS */
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  PalletViewer::printText(metricsHeight);
  glutSwapBuffers();
}

/***********************************************************************/

/* displayPictureWindow

Returned Value: none

Called By: glInit (as glutDisplayFunc for pictureWindow)

This draws the display. If dump is non-zero, it also dumps a ppm image
and sets dump to zero.

*/

static void displayPictureWindow(void) /* NO ARGUMENTS */
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  msMove(0);
  glColor3f(1.0, 1.0, 1.0);
  glCallList(stackList);
  glPopMatrix();
  if (dump)
    {
      windowDump();
      dump = 0;
    }
  glutSwapBuffers();
}

/***********************************************************************/

/* flipImage

Returned Value: none

Called By: windowDump

This flips (top to bottom) an image in a buffer written by glReadPixels
that is to be written to a ppm file. This is assuming the image has
3*width*height chars.

If the number of lines is odd, the middle line is not changed. Except for
that, each line below the middle is exchanged with the line equally far
above the middle.

*/

static void flipImage( /* ARGUMENTS                            */
 char * image,         /* an RGB image written by glReadPixels */ 
 int width,            /* image width, in pixels (not chars)   */
 int height)           /* image height, in pixels (not chars)  */
{
  char buf;
  int k;    // line increment
  int i;    // char increment
  int top;  // starting index on top
  int bot;  // starting index on bottom
  int stop;

  stop = (3 * width);
  for (k = 0; k < (height / 2); k++)
    { // swap lines
      top = (k * 3* width);
      bot = (((height - 1) - k) * 3 * width);
      for (i = 0; i < stop; i++)
	{ // swap characters
	  buf = image[top+i];
	  image[top+i] = image[bot+i];
	  image[bot+i] = buf;
	}
    }
}

/***********************************************************************/

/* glInit

Returned Value: none

Called By: main (in main.cc)

This initializes openGL, registers the callback functions, initializes
the mouse, and builds the original display list.

*/

void glInit(           /* ARGUMENTS                              */
 int argc,             /* number of arguments to the executable  */
 char* argv[],         /* array of executable name and arguments */
 const char * winName) /* name for graphics window               */
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  glutInitWindowPosition(2, 0);
  glutInitWindowSize(metricsWidth, metricsHeight);
  metricsWindow = glutCreateWindow("Pallet Metrics");
  glutReshapeFunc(reshapeMetricsWindow);
  glutDisplayFunc(displayMetricsWindow);

  glutInitWindowPosition(268, 0);
  glutInitWindowSize(pictureExtent, pictureExtent);
  pictureWindow = glutCreateWindow(winName);
  glShadeModel(GL_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glutDisplayFunc(displayPictureWindow);
  glutReshapeFunc(reshapePictureWindow);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  msInit();
  buildDisplayList();
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/********************************************************************/

/* keyboard

Returned Value: none

Called By: glInit (as glutKeyboardFunc)

If g is pressed and there are more boxes to display, another box is shown.
If f is pressed and any boxes are displayed, one is removed.
If h or H is pressed, the view is returned to its original position.
If t or T is pressed the current image is dumped to a ppm file.
if z, Z, q, or Q is pressed, palletViewer exits.

*/

static void keyboard(
 unsigned char key,
 int x,
 int y)
{
  switch (key)
    {
    case 'Q':
    case 'q':
    case 'Z':
    case 'z':
      exit(0);
      break;
    case 'h':
    case 'H':
      msInit();
      break;
    case 't':
    case 'T':
      dump = 1;
      break;
    case 'f':
      PalletViewer::recalculate(-1);
      buildDisplayList();
      glutPostRedisplay();
      glutSetWindow(metricsWindow);
      glutPostRedisplay();
      glutSetWindow(pictureWindow);
      break;
    case 'g':
      PalletViewer::recalculate(1);
      buildDisplayList();
      glutPostRedisplay();
      glutSetWindow(metricsWindow);
      glutPostRedisplay();
      glutSetWindow(pictureWindow);
      break;
    }
  glutPostRedisplay();
}

/********************************************************************/

/* makeGrid

Returned Value: none

Called By: buildDisplayList

This makes a 2 meter by 2 meter grid of light grey lines on the XY plane
spaced 0.1 meter apart. The grid is scaled down to 1 by 1 for viewing.

*/

static void makeGrid(void) /* NO ARGUMENTS */
{
  static GLfloat t;
  static GLfloat k = (GLfloat)0.5;
  static GLfloat c = (GLfloat)0.05;

  glColor3f((GLfloat)0.4, (GLfloat)0.4, (GLfloat)0.4);
  glBegin(GL_LINES);
  for (t = -k; t <= (k + 0.01); t += c)
    {
      glVertex3f(t, k, 0.0);
      glVertex3f(t, -k, 0.0);
      glVertex3f(k, t, 0.0);
      glVertex3f(-k, t, 0.0);
    }
  glEnd();
}

/********************************************************************/

/* motion

Returned Value: none

Called By: glInit (as glutMotionFunc)

This changes the display when a mouse button is held down and the
mouse is moved over the display.

If the left button is held down and the mouse is moved, the grid and
box stacks move with the cursor.

If the middle button is held down and the mouse is moved, the grid and
box stacks rotate around an axis perpendicular to the direction of cursor
motion.

If the right button is held down and the mouse is moved, the grid and
box stacks zoom away when the cursor moves up. They zoom in when the cursor
moves down. Sideways motion of the cursor does nothing. This is zooming,
not moving the point of view, so the eye never goes through boxes.

*/

static void motion( /* ARGUMENTS                                  */
 int x,             /* X location of cursor in screen coordinates */
 int y)             /* Y location of cursor in screen coordinates */
{
  msMotion(x, y, 0);
  glutPostRedisplay();
}

/***********************************************************************/

/* mouse

Returned Value: none

Called By: glInit (as glutMouseFunc)

See documentation of the motion function.

*/

static void mouse(  /* ARGUMENTS                                  */
 int button,        /* the mouse button that went up or down      */
 int state,         /* up or down                                 */
 int x,             /* X location of cursor in screen coordinates */
 int y)             /* Y location of cursor in screen coordinates */
{
  msMouse(button, state, x, y);
  glutPostRedisplay();
}

/***********************************************************************/

/* reshapeMetricsWindow

Returned Value: none

Called By: glInit (as glutReshapeFunc)

This reshapes the display. The aspect ratio of the window may change, but
the aspect ratio of the picture in the window will stay the same.

*/

static void reshapeMetricsWindow( /* ARGUMENTS                */
 int width,                       /* window width, in pixels  */
 int height)                      /* window height, in pixels */
{
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)width, 0.0, (GLdouble)height);
  metricsHeight = height;
  metricsWidth = width;
}

/***********************************************************************/

/* reshapePictureWindow

Returned Value: none

Called By: glInit (as glutReshapeFunc for pictureWindow)

This reshapes the display. The aspect ratio of the window may change, but
the aspect ratio of the picture in the window will stay the same.

*/

static void reshapePictureWindow( /* ARGUMENTS                */
 int width,                       /* window width, in pixels  */
 int height)                      /* window height, in pixels */
{
  pictureExtent = ((width < height) ? width : height);
  msReshape(pictureExtent, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -2.0);
}

/***********************************************************************/

/* windowDump

Returned Value: int
If this file is written successfully, this returns 0.
Otherwise, it prints an error message to the terminal and returns 1.

Called By: displayPictureWindow

This writes a ppm file containing an image that combines the metricsWindow
and the pictureWindow. The metricsWindow is on the left side of the
image.

This gets a copy of both windows, combines them, flips them, and
writes out the ppm file.

The indexing for making the combined window is a bit complex.

If one window is taller than the other, the part of the combined image
that is a rectangle under the shorter window is made black. 

Since glReadPixels writes pixels left to right and bottom to top,
while ppm files are read left to right and top to bottom, the image
needs to be flipped top to bottom. That is done by flipImage.


*/
#define fname_size 32
static int windowDump(void) /* NO ARGUMENTS */
{
  FILE * outFile;
  static int counter = 0;
  char fname[fname_size];
  static char allImage[IMAGE_SIZE];
  static char tempImage[IMAGE_SIZE];
  int allWidth;
  int allHeight;
  int status = 0;
  int i;            // index for lines of image
  int j;            // index for columns of image
  int k;            // index for lines of metrics image
  int stop;
  
  allWidth = (pictureExtent + metricsWidth);
  allHeight = ((pictureExtent > metricsHeight) ? pictureExtent :
		    metricsHeight);
  if ((3 * allWidth * allHeight) > IMAGE_SIZE)
    {
      fprintf(stderr, "WindowDump - Image too large for window dump\n");
      return 1;
    }
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1); // Just in case
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, pictureExtent, pictureExtent,
  	       GL_RGB, GL_UNSIGNED_BYTE, tempImage);

  stop = (3 * pictureExtent);
  if (pictureExtent > metricsHeight)
    { // picture window is taller
      for (i = 0; i < pictureExtent; i++)
	{ // copy in the picture window
	  for (j = 0; j < stop; j++)
	    allImage[(i * 3 * allWidth) + (3 * metricsWidth) + j] =
	      tempImage[(i * stop) + j];
	}
    }
  else
    { // metrics window is taller or same height
      for (i = 0; i < (metricsHeight - pictureExtent); i++)
	{ // make black rectangle under copy of picture window
	  for (j = 0; j < stop; j++)
	    allImage[(i * 3 * allWidth) + (3 * metricsWidth) + j] = 0;
	}
      for (k = 0 ; i < allHeight; i++, k++)
	{ // copy in the picture window
	  for (j = 0; j < stop; j++)
	    allImage[(i * 3 * allWidth) + (3 * metricsWidth) + j] =
	      tempImage[(k * stop) + j];
	}
    }
  glutSetWindow(metricsWindow);
  glReadBuffer(GL_BACK);
  glReadPixels(0, 0, metricsWidth, metricsHeight,
	       GL_RGB, GL_UNSIGNED_BYTE, tempImage);
  glutSetWindow(pictureWindow);
  stop = (3 * metricsWidth);
  if (pictureExtent > metricsHeight)
    { // picture window is taller
      for (i = 0; i < (pictureExtent - metricsHeight); i++)
	{ // make black rectangle under copy of metrics window
	  for (j = 0;  j < stop; j++)
	    allImage[(i * 3 * allWidth) + j] = 0;
	}
      for (k = 0 ; i < allHeight; i++, k++)
	{ // copy in the metrics window
	  for (j = 0;  j < stop; j++)
	    allImage[(i * 3 * allWidth) + j] = tempImage[(k * stop) + j];
	}
    }
  else
    { // metrics window is taller or same height
      for (i = 0; i < metricsHeight; i++)
	{
	  for (j = 0; j < stop; j++)
	    allImage[(i * 3 * allWidth) + j] = tempImage[(i * stop) + j];
	}
    }
  flipImage(allImage, allWidth, allHeight);
  snprintf(fname, fname_size, "anaglyph_%04d.ppm", counter++);
  if ((outFile = fopen(fname, "w")) == NULL)
    {
      fprintf(stderr, "WindowDump - Failed to open file for window dump\n");
      return 1;
    }
  fprintf(outFile, "P6\n%d %d\n255\n", allWidth, allHeight);
  if (fwrite(allImage, 3 * allWidth * allHeight * sizeof(char),
	     1, outFile) != 1)
    status = 1;
  fclose(outFile);
  return status;
}

/********************************************************************/

