/*
 * main.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include <stdio.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <time.h>
#include "gl/main.h"
#include "gl/view.h"
#include "xml_parser.h"
#include "packlist.h"
#include "response.h"

#define SCALE 1000.0f
// Global Variables
PackList list;
Order order;
uint c;
uint weight;
uint volume, tvolume;
double f_volume, f_tvolume, f_theight;
uint theight;

struct col {
	float r, g, b;
};
std::map <int, col> color;
PackPallet cpallet; 	// current packpallet

void pallet_draw_vector() {
	volume = 0;
	weight = 0;
	theight = 1;
	tvolume = 305*406;
	f_tvolume = (double)tvolume/SCALE;
	double density;

	for(unsigned int i = 0; i < c; i++) {
		glPushMatrix();
		Point pos = cpallet.package[i].place_position;
		Article art = cpallet.package[i].article;
		col nc = color[art.id];
		glColor3f(nc.r, nc.g, nc.b);
		glTranslated(pos.x, pos.y, pos.z);
		glScalef((float)art.width/1.0f, (float)art.length/1.0f, (float)art.height/1.0f);
		glutSolidCube(1.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glutWireCube(1.0f);
		glPopMatrix();

		if(theight < pos.z) theight = pos.z;
		weight += art.weight;
		volume += (art.height*art.width*art.length);
	}

	f_volume = (double)volume/SCALE;
	f_theight = (double)theight;
	f_tvolume *= f_theight;

	printf("\nWeight: %d\r", weight);
	printf("\nHeight: %.4lf\r", f_theight);
	printf("\nVolume: %.4lf\r", f_volume);
	printf("\nTotal Volume: %.4lf\r", f_tvolume);
	density = f_volume/f_tvolume;
	printf("\nDensity: %.12lf\r", density);
	printf("\n\r");
	glPushMatrix();
	glRasterPos2f(0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '3');
	glPopMatrix();
}

void pallet_vector_key_control(int key) {
	if(key > 0 && c < cpallet.n_package()) {
		if(key == -1) c--;
		else if(key == 1) c++;
	}
}

using namespace std;

int main(int argc, char* argv[]) {
	c = 0;
	pair<map<int, col>::iterator, bool> ret;

	order = read_order("Example.order.xml", 0);
	list = read_response("Example.packlist.xml", 0);
	cpallet = list.packpallet[0];

	srand(time(NULL));
	for (uint i = 0; i < order.n_orderline(); i++) {
		col nc;
		nc.r = (float)(rand() % 1000 + 1)/1000.0f;
		nc.g = (float)(rand() % 1000 + 1)/1000.0f;
		nc.b = (float)(rand() % 1000 + 1)/1000.0f;
		color.insert(pair<int, col> (order.orderline[i].article.id, nc));
		printf("Inserted at %d: %.2f %.2f %.2f\n", i, nc.r, nc.g, nc.b);
	}
	gl_init(argc, argv, "Pallet Viewer", 800, 600);
	glutMainLoop();

	return 0;
}
