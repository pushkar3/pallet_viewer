/*
 * response.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include "response.h"
#include "xml_parser.h"
#include <stdlib.h>
#include <fstream>

int Point::parse(std::string data) {
	x = atoi(xml_parse_tag(data, "X").c_str());
	y = atoi(xml_parse_tag(data, "Y").c_str());
	z = atoi(xml_parse_tag(data, "Z").c_str());
	return strlen(data.c_str());
}

int Package::parse(std::string data) {
	pack_sequence = atoi(xml_parse_tag(data, "PackSequence").c_str());
	incoming_sequence = atoi(xml_parse_tag(data, "IncomingSequence").c_str());
	orderlineno = atoi(xml_parse_tag(data, "OrderLineNo").c_str());
	parent_layer = atoi(xml_parse_tag(data, "ParentLayer").c_str());
	article.parse(xml_parse_tag(data, "Article"));
	barcode.parse(xml_parse_tag(data, "Barcode"));
	place_position.parse(xml_parse_tag(data, "PlacePosition"));
	orientation = atoi(xml_parse_tag(data, "Orientation").c_str());
	approach_point_1.parse(xml_parse_tag(data, "ApproachPoint1"));
	approach_point_2.parse(xml_parse_tag(data, "ApproachPoint2"));
	approach_point_3.parse(xml_parse_tag(data, "ApproachPoint3"));
	stack_height_before = atoi(xml_parse_tag(data, "StackHeightBefore").c_str());
	return strlen(data.c_str());
}

int Dimensions::parse(std::string data) {
	length = atoi(xml_parse_tag(data, "Length").c_str());
	width = atoi(xml_parse_tag(data, "Width").c_str());
	max_load_height = atoi(xml_parse_tag(data, "MaxLoadHeight").c_str());
	max_load_weight = atoi(xml_parse_tag(data, "MaxLoadWeight").c_str());
	return strlen(data.c_str());
}

int PackPallet::parse(std::string data) {
	pallet_number = atoi(xml_parse_tag(data, "PalletNumber").c_str());
	brutto_weight = atoi(xml_parse_tag(data, "BruttoWeight").c_str());
	number_of_packages = atoi(xml_parse_tag(data, "NumberOfPackages").c_str());
	description = xml_parse_tag(data, "Description");
	dimension.parse(xml_parse_tag(data, "Dimensions"));
	int ret = 1;
	while(ret) {
		std::string s_package = xml_parse_tag(data, "Package");
		if(strlen(s_package.c_str()) > 0) {
			Package p;
			p.parse(s_package);
			package.push_back(p);
		}
		ret = xml_parse_remove_first_tag(&data, "Package");
	}
	return strlen(data.c_str());
}

int PackList::parse(std::string data) {
	order_id = atoi(xml_parse_tag(data, "OrderID").c_str());
	int ret = 1;
	while(ret) {
		std::string s_packlist = xml_parse_tag(data, "PackPallet");
		if(strlen(s_packlist.c_str()) > 0) {
			PackPallet p;
			p.parse(s_packlist);
			packpallet.push_back(p);
		}
		ret = xml_parse_remove_first_tag(&data, "PackPallet");
	}
	return strlen(data.c_str());
}

PackList read_response(const char* filename, int debug) {
	std::ifstream ifs(filename);
	if(!ifs.is_open()) {
		printf("%s File not found.\n Exiting.\n", filename);
		exit(1);
	}

	int buf_len = xml_parser_get_buffer_length(filename);
	char* packlist_buf = (char*) malloc (buf_len + 1);
	ifs.read(packlist_buf, buf_len);
	std::string packlist_xml = packlist_buf;

	PackList list;
	list.parse(xml_parse_tag(packlist_xml, "PackList"));

	if(debug) {
		printf("Number of PackPallets: %d\n", list.n_packpallet());

		for(uint i = 0; i < list.n_packpallet(); i++) {
			printf("%d. Number of Packages: %d\n", list.n_packpallet(), list.packpallet[i].n_package());
			for(uint j = 0; j < list.packpallet[i].n_package(); j++) {
				printf("Pack Sequence: %d\t", list.packpallet[i].package[j].pack_sequence);
				printf("ID: %d \t Size: <%d, %d, %d>\n", list.packpallet[i].package[j].article.id,
						list.packpallet[i].package[j].article.width,
						list.packpallet[i].package[j].article.height,
						list.packpallet[i].package[j].article.length);
			}
		}
	}

	printf("PackList Read.\n");
	free(packlist_buf);
	return list;
}
