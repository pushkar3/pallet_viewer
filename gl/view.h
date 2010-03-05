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

void gl_resetview();
void gl_init(int w, int h);
void gl_resize(int w, int h);
void gl_redraw();
void gl_keyhandler(unsigned char key, int x, int y);
void gl_mousehandler(int button, int state, int x, int y);
void gl_activemousehandler(int x, int y);


#endif /* VIEW_H_ */
