/*
 * view.cpp
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 */

#include "view.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
float rotatespeed = 0.0f;     /* Each object can autorotate      */
double dtheta = 5.0;         /* Camera rotation angle increment */
CAMERA camera;
XYZ origin = {0.0,0.0,0.0};
int windowdump = 0;
int movierecord = 0;

void MakeGrid(void) {
	glPushMatrix();
	glColor3f(0.1f, 0.1f, 0.1f);
	int k = 100;
	int c = 10;
	for(int i = -k; i < k; i+=c) {
		if(i == 0) continue;
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
	glBegin(GL_LINES); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(10.0f, 0.0f, 0.0f); glEnd();
	glColor3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 10.0f, 0.0f); glEnd();
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES); glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 10.0f); glEnd();
	glPopMatrix();
}

void MakeBox(void)
{
   //XYZ pmin = {-3,-3,-3},pmax = {3,3,3};

   glColor3f(1.0,1.0,1.0);
   glutWireCube(3.0);

   /*
   glBegin(GL_LINE_STRIP);
   glVertex3f(pmin.x,pmin.y,pmin.z);
   glVertex3f(pmax.x,pmin.y,pmin.z);
   glVertex3f(pmax.x,pmin.y,pmax.z);
   glVertex3f(pmin.x,pmin.y,pmax.z);
   glVertex3f(pmin.x,pmin.y,pmin.z);
   glVertex3f(pmin.x,pmax.y,pmin.z);
   glVertex3f(pmax.x,pmax.y,pmin.z);
   glVertex3f(pmax.x,pmax.y,pmax.z);
   glVertex3f(pmin.x,pmax.y,pmax.z);
   glVertex3f(pmin.x,pmax.y,pmin.z);
   glEnd();

   glBegin(GL_LINES);
   glVertex3f(pmax.x,pmin.y,pmin.z); glVertex3f(pmax.x,pmax.y,pmin.z);
   glEnd();
   glBegin(GL_LINES);
   glVertex3f(pmax.x,pmin.y,pmax.z); glVertex3f(pmax.x,pmax.y,pmax.z);
   glEnd();
   glBegin(GL_LINES);
   glVertex3f(pmin.x,pmin.y,pmax.z); glVertex3f(pmin.x,pmax.y,pmax.z);
   glEnd();
   */
}

void CreateWorld(void) {
	static double rotateangle = 0.0;

   glPushMatrix();
   glRotatef(rotateangle,0.0,1.0,0.0);
   // MakeBox();
   DrawPallet();
   MakeGrid();
   glPopMatrix();

   rotateangle += rotatespeed;
   MakeLighting();
}

void MakeLighting(void)
{
   GLfloat fullambient[4] = {1.0,1.0,1.0,1.0};
   GLfloat position[4] = {0.0,0.0,0.0,0.0};
   GLfloat ambient[4]  = {0.2,0.2,0.2,1.0};
   GLfloat diffuse[4]  = {1.0,1.0,1.0,1.0};
   GLfloat specular[4] = {0.0,0.0,0.0,1.0};

   /* Turn off all the lights */
   glDisable(GL_LIGHT0);
   glDisable(GL_LIGHT1);
   glDisable(GL_LIGHT2);
   glDisable(GL_LIGHT3);
   glDisable(GL_LIGHT4);
   glDisable(GL_LIGHT5);
   glDisable(GL_LIGHT6);
   glDisable(GL_LIGHT7);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

   /* Turn on the appropriate lights */
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,fullambient);
   glLightfv(GL_LIGHT0,GL_POSITION,position);
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
   glEnable(GL_LIGHT0);

   /* Sort out the shading algorithm */
   glShadeModel(GL_SMOOTH);

   /* Turn lighting on */
   glEnable(GL_LIGHTING);
}


void CameraHome(int mode)
{
	mode = 4;
   camera.aperture = 60;
   camera.pr = origin;

   camera.vd.x = 1;
   camera.vd.y = 0;
   camera.vd.z = 0;

   camera.vu.x = 0;
   camera.vu.y = 0;
   camera.vu.z = 1;

   camera.vp.x = -10;
   camera.vp.y = 200;
   camera.vp.z = 50;

   switch (mode) {
   case 0:
   case 2:
   case 4:
      camera.focallength = 10;
      break;
   case 1:
      camera.focallength = 5;
      break;
   case 3:
      camera.focallength = 15;
      break;
   }

   /* Non stressful stereo setting */
   camera.eyesep = camera.focallength / 30.0;
   if (mode == 4)
      camera.eyesep = 0;
}

int WindowDump(int width,int height)
{
   FILE *fptr;
   static int counter = 0;
   int status = TRUE;
   char fname[32];
   char *image;

   /* Allocate our buffer for the image */
   if ((image = (char*)malloc(3*width*height*sizeof(char))) == NULL) {
      fprintf(stderr,"WindowDump - Failed to allocate memory for image\n");
      return(FALSE);
   }

   glPixelStorei(GL_PACK_ALIGNMENT,1); /* Just in case */

   sprintf(fname,"anaglyph_%04d.ppm",counter++);
   if ((fptr = fopen(fname,"w")) == NULL) {
      fprintf(stderr,"WindowDump - Failed to open file for window dump\n");
      return(FALSE);
   }

   /* Copy the image into our buffer */
   glReadBuffer(GL_BACK);
   glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);

   /* Write the file */
   fprintf(fptr,"P6\n%d %d\n255\n",width,height);
   if (fwrite(image,3*width*height*sizeof(char),1,fptr) != 1)
      status = FALSE;
   fclose(fptr);

   free(image);
   return(status);
}


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

	glutDisplayFunc(handle_display);
	glutReshapeFunc(handle_reshape);
	glutVisibilityFunc(handle_visibility);
	glutKeyboardFunc(handle_keyboard);
	glutSpecialFunc(handle_specialkeyboard);
	glutMouseFunc(handle_mouse);
	glutMotionFunc(handle_mousemotion);

	CreateEnvironment();
	CameraHome(0);

}

void handle_display(void)
{
   XYZ right,focus;

   /* Determine the focal XYZ */
   Normalise(&camera.vd);
   focus.x = camera.vp.x + camera.focallength * camera.vd.x;
   focus.y = camera.vp.y + camera.focallength * camera.vd.y;
   focus.z = camera.vp.z + camera.focallength * camera.vd.z;

   /* Derive the the "right" vector */
   CROSSPROD(camera.vd,camera.vu,right);
   Normalise(&right);
   right.x *= camera.eyesep / 2.0;
   right.y *= camera.eyesep / 2.0;
   right.z *= camera.eyesep / 2.0;

   /* Set the buffer for writing and reading */
   glDrawBuffer(GL_BACK);
   glReadBuffer(GL_BACK);

   /* Clear things */
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   /* DVV */
   /*glClear(GL_ACCUM_BUFFER_BIT);*/ /* Not strictly necessary */

   /* Set projection */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(camera.aperture,
      camera.screenwidth/(double)camera.screenheight,0.1,10000.0);
   glViewport(0,0,camera.screenwidth,camera.screenheight);

   /* Left eye filter */
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
   switch (glassestype) {
   case REDBLUE:
   case REDGREEN:
   case REDCYAN:
      glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
      break;
   case BLUERED:
      glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
      break;
   case GREENRED:
      glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
      break;
   case CYANRED:
      glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
      break;
   }

   /* Create the model */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camera.vp.x - right.x,
             camera.vp.y - right.y,
             camera.vp.z - right.z,
             focus.x,focus.y,focus.z,
             camera.vu.x,camera.vu.y,camera.vu.z);
   CreateWorld();
   glFlush();
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

   /* DVV */
   /* Write over the accumulation buffer */
   /*glAccum(GL_LOAD,1.0);*/ /* Could also use glAccum(GL_ACCUM,1.0); */

   glDrawBuffer(GL_BACK);
   /*
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   */
   glClear(GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(camera.aperture,
      camera.screenwidth/(double)camera.screenheight,0.1,10000.0);
   glViewport(0,0,camera.screenwidth,camera.screenheight);

   /* Right eye filter */
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
   switch (glassestype) {
   case REDBLUE:
      glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
      break;
   case REDGREEN:
      glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
      break;
   case REDCYAN:
      glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
      break;
   case BLUERED:
   case GREENRED:
   case CYANRED:
      glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
      break;
   }

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(camera.vp.x + right.x,
             camera.vp.y + right.y,
             camera.vp.z + right.z,
             focus.x,focus.y,focus.z,
             camera.vu.x,camera.vu.y,camera.vu.z);
   CreateWorld();
   glFlush();
   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

   /* Addin the new image and copy the result back */
   /* DVV
   glAccum(GL_ACCUM,1.0);
   glAccum(GL_RETURN,1.0);
   */

   /* Optionally dump image */
   if (windowdump || movierecord) {
      WindowDump(camera.screenwidth,camera.screenheight);
      windowdump = 0;
   }

   /* Let's look at it */
   glutSwapBuffers();
}

void HandleTimer(int value)
{
   glutPostRedisplay();
   glutTimerFunc(30, HandleTimer, 0);
}

void handle_visibility(int visible)
{
   if (visible == GLUT_VISIBLE)
      HandleTimer(0);
}

void handle_reshape(int w,int h)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glViewport(0,0,(GLsizei)w,(GLsizei)h);
   camera.screenwidth = w;
   camera.screenheight = h;
}

void handle_specialkeyboard(int key,int x, int y)
{
   switch (key) {
   case GLUT_KEY_LEFT:
      RotateCamera(-1,0,0);
      break;
   case GLUT_KEY_RIGHT:
      RotateCamera(1,0,0);
      break;
   case GLUT_KEY_UP:
      RotateCamera(0,1,0);
      break;
   case GLUT_KEY_DOWN:
      RotateCamera(0,-1,0);
      break;
   }
}


void handle_keyboard(unsigned char key,int x, int y) {
   switch (key) {
   case 'Z':
   case 'z':
      exit(0);
      break;
   case 'h':                           /* Go home     */
   case 'H':
      CameraHome(0);
      break;
   case 't':                           /* Save one image */
   case 'T':
      windowdump = 1;
      break;
   case 'r':                           /* Toggle image recording */
   case 'R':
      movierecord = !movierecord;
      break;
   case '[':                           /* Roll anti clockwise */
      RotateCamera(0,0,-1);
      break;
   case ']':                           /* Roll clockwise */
      RotateCamera(0,0,1);
      break;
   case 'w':                           /* Translate camera up */
   case 'W':
      TranslateCamera(0,10, 0);
      break;
   case 's':                           /* Translate camera down */
   case 'S':
      TranslateCamera(0,-10, 0);
      break;
   case 'a':                           /* Translate camera left */
   case 'A':
      TranslateCamera(-10,0, 0);
      break;
   case 'd':                           /* Translate camera right */
   case 'D':
      TranslateCamera(10,0, 0);
      break;
   case 'q':                           /* Zoom in */
   case 'Q':
      TranslateCamera(0,0, 10);
      break;
   case 'e':                           /* Zoom out */
   case 'E':
	  TranslateCamera(0,0, -10);
	  break;
   case 'f':
	   Pallet_Keyboard_Control(-1);
	   break;
   case 'g':
	   Pallet_Keyboard_Control(1);
	   break;
   }
}


void handle_mouse(int button,int state,int x,int y)
{
   if (state == GLUT_DOWN) {
      if (button == GLUT_LEFT_BUTTON) {
         currentbutton = GLUT_LEFT_BUTTON;
      } else if (button == GLUT_MIDDLE_BUTTON) {
         currentbutton = GLUT_MIDDLE_BUTTON;
      }
   }
}

void handle_mousemotion(int x,int y)
{
   static int xlast=-1,ylast=-1;
   int dx,dy;

   dx = x - xlast;
   dy = y - ylast;
   if (dx < 0)      dx = -1;
   else if (dx > 0) dx =  1;
   if (dy < 0)      dy = -1;
   else if (dy > 0) dy =  1;

   if (currentbutton == GLUT_LEFT_BUTTON)
      RotateCamera(-dx,dy,0);
   else if (currentbutton == GLUT_MIDDLE_BUTTON)
      RotateCamera(0,0,dx);

   xlast = x;
   ylast = y;
}

/*
   Normalise a vector
*/
void Normalise(XYZ *p)
{
   double length;

   length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
   if (length != 0) {
      p->x /= length;
      p->y /= length;
      p->z /= length;
   } else {
      p->x = 0;
      p->y = 0;
      p->z = 0;
   }
}

/*
   Calculate the unit normal at p given two other XYZs
   p1,p2 on the surface. The normal XYZs in the direction
   of p1 crossproduct p2
*/
XYZ CalcNormal(XYZ p,XYZ p1, XYZ p2)
{
   XYZ n,pa,pb;

   pa.x = p1.x - p.x;
   pa.y = p1.y - p.y;
   pa.z = p1.z - p.z;
   pb.x = p2.x - p.x;
   pb.y = p2.y - p.y;
   pb.z = p2.z - p.z;
   Normalise(&pa);
   Normalise(&pb);

   n.x = pa.y * pb.z - pa.z * pb.y;
   n.y = pa.z * pb.x - pa.x * pb.z;
   n.z = pa.x * pb.y - pa.y * pb.x;
   Normalise(&n);

   return(n);
}

/*
   Rotate (ix,iy) or roll (iz) the camera about the focal XYZ
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(int ix,int iy,int iz)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radius,dd,radians;
   double dx,dy,dz;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;

   /* Handle the roll */
   if (iz != 0) {
      camera.vu.x += iz * right.x * radians;
      camera.vu.y += iz * right.y * radians;
      camera.vu.z += iz * right.z * radians;
      Normalise(&camera.vu);
      return;
   }

   /* Distance from the rotate XYZ */
   dx = camera.vp.x - camera.pr.x;
   dy = camera.vp.y - camera.pr.y;
   dz = camera.vp.z - camera.pr.z;
   radius = sqrt(dx*dx + dy*dy + dz*dz);

   /* Determine the new view XYZ */
   dd = radius * radians;
   newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
   newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
   newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
   Normalise(&newvp);
   camera.vp.x = camera.pr.x + radius * newvp.x;
   camera.vp.y = camera.pr.y + radius * newvp.y;
   camera.vp.z = camera.pr.z + radius * newvp.z;

   /* Determine the new right vector */
   newr.x = camera.vp.x + right.x - camera.pr.x;
   newr.y = camera.vp.y + right.y - camera.pr.y;
   newr.z = camera.vp.z + right.z - camera.pr.z;
   Normalise(&newr);
   newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
   newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
   newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

   camera.vd.x = camera.pr.x - camera.vp.x;
   camera.vd.y = camera.pr.y - camera.vp.y;
   camera.vd.z = camera.pr.z - camera.vp.z;
   Normalise(&camera.vd);

   /* Determine the new up vector */
   CROSSPROD(newr,camera.vd,camera.vu);
   Normalise(&camera.vu);
}

/*
   Translate (pan) the camera view XYZ
   In response to i,j,k,l keys
   Also move the camera rotate location in parallel
*/
void TranslateCamera(int ix,int iy, int iz)
{
   XYZ vp,vu,vd;
   XYZ right;
   double radians,delta;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;
   delta = dtheta * camera.focallength / 90.0;

   camera.vp.x += iy * vu.x * delta;
   camera.vp.y += iy * vu.y * delta;
   camera.vp.z += iy * vu.z * delta;
   camera.pr.x += iy * vu.x * delta;
   camera.pr.y += iy * vu.y * delta;
   camera.pr.z += iy * vu.z * delta;

   camera.vp.x += ix * right.x * delta;
   camera.vp.y += ix * right.y * delta;
   camera.vp.z += ix * right.z * delta;
   camera.pr.x += ix * right.x * delta;
   camera.pr.y += ix * right.y * delta;
   camera.pr.z += ix * right.z * delta;

   camera.vp.x += iz * vd.x * delta;
   camera.vp.y += iz * vd.y * delta;
   camera.vp.z += iz * vd.z * delta;
   camera.pr.x += iz * vd.x * delta;
   camera.pr.y += iz * vd.y * delta;
   camera.pr.z += iz * vd.z * delta;
}

void CreateEnvironment(void)
{
   int num[2];

   glEnable(GL_DEPTH_TEST);
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   glDisable(GL_POLYGON_SMOOTH);
   glDisable(GL_DITHER);
   glDisable(GL_CULL_FACE);
   glDisable(GL_BLEND);    /* Not necessary but for bug in PS350 driver */
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glLineWidth(1.0);
   glPointSize(1.0);
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glFrontFace(GL_CW);
   glClearColor(0.0,0.0,0.0,0.0);
   glClearAccum(0.0,0.0,0.0,0.0);   /* The default */
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glPixelStorei(GL_UNPACK_ALIGNMENT,1);

   /*
      In case you want to check what the colour depth of your
      accumulation buffer is, hopefully it is 16 bits
   */
   if (debug) {
      glGetIntegerv(GL_ACCUM_RED_BITS,num);
      fprintf(stderr,"Red bits: %d\n",num[0]);
      glGetIntegerv(GL_ACCUM_GREEN_BITS,num);
      fprintf(stderr,"Green bits: %d\n",num[0]);
      glGetIntegerv(GL_ACCUM_BLUE_BITS,num);
      fprintf(stderr,"Blue bits: %d\n",num[0]);
      glGetIntegerv(GL_ACCUM_ALPHA_BITS,num);
      fprintf(stderr,"Alpha bits: %d\n",num[0]);
   }
}
