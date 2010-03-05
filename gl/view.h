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
} Point;

typedef struct {
   Point vp;              /* View position           */
   Point vd;              /* View direction vector   */
   Point vu;              /* View up direction       */
   Point pr;              /* Point to rotate about   */
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


#endif /* VIEW_H_ */
