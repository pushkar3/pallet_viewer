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
#include "xml_parser.h"
#include "packlist.h"
#include "response.h"
#include "gl/view.h"

using namespace std;

typedef struct {
	int x, y, z;
	int width, length, height;
} PalletProperties;
int max_x, max_y, max_z;

vector<PalletProperties> v_pallet;
int c;

void packlist_unittest(const char* filename) {
	ifstream ifs(filename);
	if(!ifs.is_open()) {
		printf("File not open.\n");
		return;
	}

	int buf_len = xml_parser_get_buffer_length(filename);
	char* packlist_buf = (char*) malloc (buf_len + 1);
	ifs.read(packlist_buf, buf_len);
	std::string packlist_xml = packlist_buf;

	PackList list;
	list.parse(xml_parse_tag(packlist_xml, "PackList"));

	printf("Number of PackPallets: %d\n", list.n_packpallet());

	for(uint i = 0; i < list.n_packpallet(); i++) {
		printf("%d. Number of Packages: %d\n", list.n_packpallet(), list.packpallet[i].n_package());
		for(uint j = 0; j < list.packpallet[i].n_package(); j++) {
			printf("Pack Sequence: %d\n", list.packpallet[i].package[j].pack_sequence);
			printf("ID: %d <%d, %d, %d>\n", list.packpallet[i].package[j].article.id,
					list.packpallet[i].package[j].article.width,
					list.packpallet[i].package[j].article.height,
					list.packpallet[i].package[j].article.length);
		}
	}

	printf("PackList UnitTest Done.\n");
}

void DrawRectangularCube(int x, int y, int z, int width, int length, int height) {
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

void DrawPallet() {
	for(int i = 0; i < c; i++) {
		DrawRectangularCube(v_pallet[i].x, v_pallet[i].y, v_pallet[i].z, v_pallet[i].width, v_pallet[i].length, v_pallet[i].height);
	}
}

void Pallet_Keyboard_Control(int key) {
	if(key > 0 && key < v_pallet.size()) {
		if(key == -1) c--;
		else if(key == 1) c++;
	}
}


void MakePallet() {
	ifstream ifs("Example.packlist.xml");
	if(!ifs.is_open()) {
		printf("File not open.\n");
		return;
	}

	int buf_len = xml_parser_get_buffer_length("Example.packlist.xml");
	char* packlist_buf = (char*) malloc (buf_len + 1);
	ifs.read(packlist_buf, buf_len);
	std::string packlist_xml = packlist_buf;

	PackList list;
	list.parse(xml_parse_tag(packlist_xml, "PackList"));

	printf("Number of PackPallets: %d\n", list.n_packpallet());

	for(uint i = 0; i < list.n_packpallet(); i++) {
		printf("%d. Number of Packages: %d\n", list.n_packpallet(), list.packpallet[i].n_package());
		for(uint j = 0; j < list.packpallet[i].n_package(); j++) {
			printf("Pack Sequence: %d\n", list.packpallet[i].package[j].pack_sequence);
			printf("Adding ID: %d <%d, %d, %d> at <%d, %d, %d>\n", list.packpallet[i].package[j].article.id,
					list.packpallet[i].package[j].place_position.x,
					list.packpallet[i].package[j].place_position.y,
					list.packpallet[i].package[j].place_position.z,
					list.packpallet[i].package[j].article.width,
					list.packpallet[i].package[j].article.height,
					list.packpallet[i].package[j].article.length);
			PalletProperties p;
			p.x = list.packpallet[i].package[j].place_position.x;
			p.y = list.packpallet[i].package[j].place_position.y;
			p.z = list.packpallet[i].package[j].place_position.z;
			p.width = list.packpallet[i].package[j].article.width;
			p.height = list.packpallet[i].package[j].article.height;
			p.length = list.packpallet[i].package[j].article.length;
			v_pallet.push_back(p);
		}
	}

	printf("PackList UnitTest Done.\n");
}

int main(int argc, char* argv[]) {
	c = 0;
#ifdef PACKLIST_UNITTEST
	packlist_unittest("Example.packlist.xml");
#else
	MakePallet();
	gl_init(argc, argv, 800, 600);
	glutMainLoop();
	return(0);
#endif
	return 0;
}
