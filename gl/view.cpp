/*
 * view.cpp
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 *      Brief: Viewer for opengl (Edit main)
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glcamera.h"
#include "view.h"
#include "main.h"

typedef enum {REDBLUE = 1, REDGREEN, REDCYAN, BLUERED, GREENRED, CYANRED} Glasses;

int glassestype = REDBLUE;
int debug = 0;
int fullscreen = 0;
int currentbutton = -1;
int windowdump = 0;
int movierecord = 0;
int rotate = 0;

glcamera* camera;

void MakeGrid(void) {
	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.3f);
	int k = 1000;
	int c = 100;
	for (int i = -k; i < k; i += c) {
		glBegin(GL_LINES);
		glVertex3f(i, k, 0.0f);
		glVertex3f(i, -k, 0.0f);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(k, i, 0.0f);
		glVertex3f(-k, i, 0.0f);
		glEnd();
	}

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glEnd();
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glEnd();
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10.0f);
	glEnd();
	glPopMatrix();
}

void CreateWorld(void) {
	glPushMatrix();
	MakeGrid();
	_draw();
	glPopMatrix();
	// MakeLighting();
}



int WindowDump(int width, int height) {
	FILE *fptr;
	static int counter = 0;
	int status = 0;
	char fname[32];
	char *image;

	if ((image = (char*) malloc(3 * width * height * sizeof(char))) == NULL) {
		fprintf(stderr, "WindowDump - Failed to allocate memory for image\n");
		return (0);
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1); /* Just in case */

	sprintf(fname, "anaglyph_%04d.ppm", counter++);
	if ((fptr = fopen(fname, "w")) == NULL) {
		fprintf(stderr, "WindowDump - Failed to open file for window dump\n");
		return (0);
	}

	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

	fprintf(fptr, "P6\n%d %d\n255\n", width, height);
	if (fwrite(image, 3 * width * height * sizeof(char), 1, fptr) != 1)
		status = 0;
	fclose(fptr);

	free(image);
	return (status);
}

void gl_init(int argc, char* argv[], const char* win_name, int w, int h) {
	glutInit(&argc, argv);
	camera = new glcamera();
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutCreateWindow(win_name);
	camera->screenwidth = w;
	camera->screenheight = h;
	glutReshapeWindow(camera->screenwidth, camera->screenheight);
	if (fullscreen)
		glutFullScreen();

	glutDisplayFunc(handle_simple_display);
	glutReshapeFunc(handle_reshape);
	glutVisibilityFunc(handle_visibility);
	glutKeyboardFunc(handle_keyboard);
	glutSpecialFunc(handle_specialkeyboard);
	glutMouseFunc(handle_mouse);
	glutMotionFunc(handle_mousemotion);
	// CreateEnvironment();
}

void handle_simple_display(void) {
	Vec3 focus = camera->get_focalvector();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->aperture, camera->screenwidth/(double) camera->screenheight, 0.1, 10000.0);
	glViewport(0, 0, camera->screenwidth, camera->screenheight);

	// Create the model

	gluLookAt(camera->vpos.x, camera->vpos.y, camera->vpos.z, focus.x, focus.y, focus.z,
	camera->vup.x, camera->vup.y, camera->vup.z);
	glShadeModel(GL_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	CreateWorld();

	if (windowdump || movierecord) {
		WindowDump(camera->screenwidth, camera->screenheight);
		windowdump = 0;
	}

	glutSwapBuffers();
}

void handle_display(void) {
	Vec3 focus = camera->get_focalvector();
	Vec3 right = camera->get_sidevector();

	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->aperture, camera->screenwidth/(double) camera->screenheight, 0.1, 10000.0);
	glViewport(0, 0, camera->screenwidth, camera->screenheight);

	// Left eye filter
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	switch (glassestype) {
	case REDBLUE:
	case REDGREEN:
	case REDCYAN:
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		break;
	case BLUERED:
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		break;
	case GREENRED:
		glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		break;
	case CYANRED:
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		break;
	}

	// Create the model
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->vpos.x - right.x, camera->vpos.y - right.y,
			camera->vpos.z - right.z, focus.x, focus.y, focus.z, camera->vup.x,
			camera->vup.y, camera->vup.z);
	CreateWorld();
	//glFlush();
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glDrawBuffer(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->aperture, camera->screenwidth/(double) camera->screenheight, 0.1, 10000.0);
	glViewport(0, 0, camera->screenwidth, camera->screenheight);

	// Right eye filter
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	switch (glassestype) {
	case REDBLUE:
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		break;
	case REDGREEN:
		glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		break;
	case REDCYAN:
		glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
		break;
	case BLUERED:
	case GREENRED:
	case CYANRED:
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		break;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->vpos.x + right.x, camera->vpos.y + right.y,
			camera->vpos.z + right.z, focus.x, focus.y, focus.z, camera->vup.x,
			camera->vup.y, camera->vup.z);
	CreateWorld();
	glFlush();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	if (windowdump || movierecord) {
		WindowDump(camera->screenwidth, camera->screenheight);
		windowdump = 0;
	}

	glutSwapBuffers();
}

void HandleTimer(int value) {
	glutPostRedisplay();
	glutTimerFunc(30, HandleTimer, 0);
}

void handle_visibility(int visible) {
	if (visible == GLUT_VISIBLE)
		HandleTimer(0);
}

void handle_reshape(int w, int h) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	camera->screenwidth = w;
	camera->screenheight = h;
}

void handle_keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'Q':
	case 'q':
	case 'Z':
	case 'z':
		exit(0);
		break;
	case 'h':
	case 'H':
		camera->init();
		break;
	case 't':
	case 'T':
		windowdump = 1;
		break;
	case 'r': // Toggle image recording
	case 'R':
		movierecord = !movierecord;
		break;
	case 'w': // Translate camera up
	case 'W':
		camera->key_up();
		break;
	case 's': // Translate camera down
	case 'S':
		camera->key_down();
		break;
	case 'a': // Translate camera left
	case 'A':
		camera->key_left();
		break;
	case 'd': // Translate camera right
	case 'D':
		camera->key_right();
		break;
	case 'o':
	case 'O':
		rotate = !rotate;
	default:
		_keyboard(key);		// Check for user-defined key controls
	}
}

void handle_specialkeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		camera->key_left();
		break;
	case GLUT_KEY_RIGHT:
		camera->key_right();
		break;
	case GLUT_KEY_UP:
		camera->key_up();
		break;
	case GLUT_KEY_DOWN:
		camera->key_down();
		break;
	}
}

void handle_mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			currentbutton = GLUT_LEFT_BUTTON;
		} else if (button == GLUT_MIDDLE_BUTTON) {
			currentbutton = GLUT_MIDDLE_BUTTON;
		}
	}
}

void handle_mousemotion(int x, int y) {
	static int xlast = -1, ylast = -1;
	int dx, dy;

	dx = xlast - x;
	dy = y - ylast;

	camera->mouse(dx, dy);

	xlast = x;
	ylast = y;
}

void CreateEnvironment(void) {
	int num[2];

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND); // Not necessary but for bug in PS350 driver
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.0);
	glPointSize(1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CW);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearAccum(0.0, 0.0, 0.0, 0.0); // The default
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (debug) {
		glGetIntegerv(GL_ACCUM_RED_BITS, num);
		fprintf(stderr, "Red bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_GREEN_BITS, num);
		fprintf(stderr, "Green bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_BLUE_BITS, num);
		fprintf(stderr, "Blue bits: %d\n", num[0]);
		glGetIntegerv(GL_ACCUM_ALPHA_BITS, num);
		fprintf(stderr, "Alpha bits: %d\n", num[0]);
	}
}

void MakeLighting(void) {
	GLfloat fullambient[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat position[4] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[4] = { 0.0, 0.0, 0.0, 1.0 };

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fullambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
}

void push_2d_mode() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, camera->screenwidth, 0, camera->screenheight);
	glScalef(1.0f, -1.0f, 1.0f);
	glTranslatef(0.0f, -1.0f*camera->screenheight, 0.0f);
	glMatrixMode(GL_MODELVIEW);
}

void pop_2d_mode() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw_string(float x, float y, void* font, char* string) {
	push_2d_mode();
	char *c;
	glRasterPos2f(x, y);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
	pop_2d_mode();
}

