/*
 * main.cpp
 *
 *  Created on: Mar 10, 2010
 *      Author: pushkar
 */

#include "view.h"
#include "main.h"

void draw_wired_cube(int x, int y, int z, int width, int length, int height) {
	glBegin(GL_LINES);
	glVertex3d(x, y, z); glVertex3d(x+width, y, z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y, z); glVertex3d(x, y+length, z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y, z); glVertex3d(x, y, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x+width, y, z); glVertex3d(x+width, y+length, z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x+width, y, z); glVertex3d(x+width, y, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y+length, z); glVertex3d(x+width, y+length, z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y+length, z); glVertex3d(x, y+length, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y, z+height); glVertex3d(x, y+length, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y, z+height); glVertex3d(x+width, y, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x+width, y, z+height); glVertex3d(x+width, y+length, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x+width, y+length, z); glVertex3d(x+width, y+length, z+height);
	glEnd();
	glBegin(GL_LINES);
	glVertex3d(x, y+length, z+height); glVertex3d(x+width, y+length, z+height);
	glEnd();
}



void _keyboard(unsigned char key) {
	switch (key) {
	case 'f':
		pallet_vector_key_control(-1);
		break;
	case 'g':
		pallet_vector_key_control(1);
		break;
	}
}

void _draw() {
	pallet_draw_vector();
}
