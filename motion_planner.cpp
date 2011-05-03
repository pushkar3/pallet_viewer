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

	if(argc < 5) {
		printf("usage: %s -o Example.order.xml -p Example.packlist.xml\n", argv[0]);
		printf("check: man pallet_viewer for more information.\n");
		if(argc < 3) {
			printf("Did you try the example xml's in /usr/share/vmac/?\n\n");
		}
		exit(0);
	}

	for(int i = 1; i < argc; i+=2) {
		if(strcmp(argv[i], "-o") == 0) strcpy(order_file, argv[i+1]);
		if(strcmp(argv[i], "-p") == 0) strcpy(packlist_file, argv[i+1]);
	}

	oxml.parse(order_file, 0, 0);
	list = read_response(packlist_file, 0);
	cpallet = list.packpallet[0];
	pallet = oxml.pallet[0];

	printf("Done.\n");
	return 0;
}
