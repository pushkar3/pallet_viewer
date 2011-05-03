/*
 * motion_planner.cpp
 *
 *  Created on: May 3, 2011
 *      Author: pushkar
 */

#include <stdio.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <time.h>
#include <math.h>
#include "xml_parser.h"
#include "packlist.h"
#include "response.h"

// Global Variables
PackList list;
OrderXML oxml;
PackPallet cpallet; 	// current packpallet from Response XML
Pallet pallet;			// current pallet from Order XML

using namespace std;

int main(int argc, char* argv[]) {
	char order_file[100] = "";
	char packlist_file[100] = "";
	char packlist_file_out[100] = "out.xml";

	if(argc < 6) {
		printf("usage: %s -o Example.order.xml -p Example.packlist.xml -v Output.packlist.xml\n", argv[0]);
		printf("check: man pallet_viewer for more information.\n");
		if(argc < 6) {
			printf("Did you try the example xml's in /u	sr/share/vmac/?\n\n");
		}
		exit(0);
	}

	for(int i = 1; i < argc; i+=2) {
		if(strcmp(argv[i], "-o") == 0) strcpy(order_file, argv[i+1]);
		if(strcmp(argv[i], "-p") == 0) strcpy(packlist_file, argv[i+1]);
		if(strcmp(argv[i], "-v") == 0) strcpy(packlist_file_out, argv[i+1]);
	}

	oxml.parse(order_file, 0, 0);
	list = read_response(packlist_file, 0);
	cpallet = list.packpallet[0];
	pallet = oxml.pallet[0];

	// Set params
	// Heights of approach points 1, 2 and 3 (lowest)

	int h2 = 0; // Set this to max height of the highest block
	for (uint i = 0; i < list.n_packpallet(); i++) {
		PackPallet p = list.packpallet[i];
		for (uint j = 0; j < p.n_package(); j++) {
			if (h2 < (int)p.package[j].article.height)
				h2 = p.package[j].article.height;
		}
	}
	int h1 = h2+40;
	int h3 = 3;
	int far = 60;

	// Planning
	for (uint i = 0; i < list.n_packpallet(); i++) {
		PackPallet *p = &(list.packpallet[i]);
		for (uint j = 0; j < p->n_package(); j++) {
				p->package[j].approach_point_3.set(0, 0, h3); // constant

				int x = 0, y = 0;
				Point pos = p->package[j].place_position;
				int height = p->package[j].article.height;
				int com_x = 0;
				int com_y = 0;
				int com_count = 0;
				for (uint n = 0; n < j; n++) {
					if(abs(p->package[n].place_position.z - pos.z) < height) {
						com_x += (pos.x - p->package[n].place_position.x);
						com_y += (pos.y - p->package[n].place_position.y);
						com_count++;
					}
				}
				if(com_count > 0) {
					com_x /= com_count;
					com_y /= com_count;
				}
				int obstacles = atan2(com_y, com_x);
				x = cos(obstacles)*far;
				y = sin(obstacles)*far;

				p->package[j].approach_point_2.set(x, y, h2);
				p->package[j].approach_point_1.set(x, y, h1);
		}
	}

	write_response(list, packlist_file_out);
	printf("Done.\n");
	return 0;
}
