/*
 * view.h
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 *      Brief: Viewer for opengl (Edit main)
 */

#ifndef VIEW_H_
#define VIEW_H_

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

extern int gl_window;

void gl_init(int argc, char* argv[], const char* win_name, int w, int h);

void handle_display();
void handle_simple_display();
void handle_keyboard(unsigned char key, int x, int y);
void handle_specialkeyboard(int key, int x, int y);
void handle_mouse(int, int, int, int);
void handle_visibility(int vis);
void handle_reshape(int, int);
void handle_mousemotion(int, int);

void MakeLighting(void);
void CreateEnvironment(void);

int WindowDump(int, int);

void push_2d_mode();
void pop_2d_mode();
void draw_string(float x, float y, void* font, char* string);


#endif /* VIEW_H_ */
