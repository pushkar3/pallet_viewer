/*
 * view.cpp
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 */

#include "view.h"

#define REDBLUE  1
#define REDGREEN 2
#define REDCYAN  3
#define BLUERED  4
#define GREENRED 5
#define CYANRED  6
int glassestype = REDBLUE;

int debug = 0;
int fullscreen = 0;
int currentbutton = -1;
float rotatespeed = 0.5;     /* Each object can autorotate      */
double dtheta = 1.0;         /* Camera rotation angle increment */
CAMERA camera;
Point origin = {0.0,0.0,0.0};

void gl_init(int argc, char* argv[], int w, int h) {
	glutInit(&argc, argv);
	/* DVV - removed GLUT_ACCUM */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	/* Create the window and handlers */
	glutCreateWindow("Pallet Viewer");
	camera.screenwidth = w;
	camera.screenheight = h;
	glutReshapeWindow(camera.screenwidth,camera.screenheight);
	if (fullscreen)
	  glutFullScreen();
	/*
	glutDisplayFunc(handle_display);
	glutReshapeFunc(handle_reshape);
	glutVisibilityFunc(handle_visibility);
	glutKeyboardFunc(handle_keyboard);
	glutSpecialFunc(handle_specialkeyboard);
	glutMouseFunc(handle_mouse);
	glutMotionFunc(handle_mousemotion);
	*/
}
