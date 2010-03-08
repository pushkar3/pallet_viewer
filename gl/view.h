/*
 * view.h
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

extern int gl_window;

typedef struct {
   double x,y,z;
} XYZ;

typedef struct {
   unsigned char r,g,b,a;
} PIXELA;
typedef struct {
   double r,g,b;
} COLOUR;

#define ABS(x) (x < 0 ? -(x) : (x))
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define SIGN(x) (x < 0 ? (-1) : 1)
#define MODULUS(p) (sqrt(p.x*p.x + p.y*p.y + p.z*p.z))
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x


typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   XYZ pr;              /* XYZ to rotate about   */
   double focallength;  /* Focal Length along vd   */
   double aperture;     /* Camera aperture         */
   double eyesep;       /* Eye separation          */
   int screenheight,screenwidth;
} CAMERA;

void gl_resetview();
void gl_init(int argc, char* argv[], int w, int h);
void gl_resize(int w, int h);
void gl_redraw();
void gl_keyhandler(unsigned char key, int x, int y);
void gl_mousehandler(int button, int state, int x, int y);
void gl_activemousehandler(int x, int y);

void handle_display(void);
void handle_keyboard(unsigned char key,int x, int y);
void handle_specialkeyboard(int key,int x, int y);
void handle_mouse(int,int,int,int);
void handle_visibility(int vis);
void handle_reshape(int,int);
void handle_mousemotion(int,int);
void handle_passivemotion(int,int);
void MakeLighting(void);
void CreateEnvironment(void);
void MakeBox(void);
void DrawPallet(void);

void Pallet_Keyboard_Control(int);

void RotateCamera(int,int,int);
void TranslateCamera(int,int,int);
void CameraHome(int);
int  WindowDump(int,int);
void Normalise(XYZ *);
XYZ  CalcNormal(XYZ,XYZ,XYZ);

#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322
#define ESC 27
#define FALSE 0
#define TRUE 1

#endif /* VIEW_H_ */
