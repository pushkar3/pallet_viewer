/*
 * main.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include "xml_parser.h"
#include "packlist.h"
#include "response.h"

using namespace std;

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
		printf("%d. Numer of Packages: %d\n", list.n_packpallet(), list.packpallet[i].n_package());
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

int main(int argc, char* argv[]) {
#ifdef PACKLIST_UNITTEST
	packlist_unittest("Beverage_001.packlist.xml");
#else

#endif
	return 0;
}
