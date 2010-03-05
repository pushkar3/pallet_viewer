/*
 * view.cpp
 *
 *  Created on: Mar 5, 2010
 *      Author: pushkar
 */

#include "view.h"

GLuint create_Grid_DL() {
	float g = 10.0f;
	GLuint Grid_DL;
	Grid_DL = glGenLists(1);
	glNewList(Grid_DL, GL_COMPILE);
		glPushMatrix();
		glColor3f(0.3f, 0.3f, 0.3f);
		for(float i = -g*10; i < g*10+1; i++) {
			glBegin(GL_LINES);
			glVertex3f(i/10, -g, 0);
			glVertex3f(i/10, g, 0);
			glEnd();
		}
		for(float i = -g*10; i < g*10+1; i++) {
			glBegin(GL_LINES);
			glVertex3f(-g, i/10, 0);
			glVertex3f(g, i/10, 0);
			glEnd();
		}
		glPopMatrix();
	glEndList();
	return Grid_DL;
}

void gl_init(int w, int h) {
}
