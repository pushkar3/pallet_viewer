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

std::string Point::xml(const char* name) {
	std::string ret("");
	ret.append(xml_start_tag(name));
	ret.append(xml_make_tag("X", itoa(x).c_str()));
	ret.append(xml_make_tag("Y", itoa(y).c_str()));
	ret.append(xml_make_tag("Z", itoa(z).c_str()));
	ret.append(xml_end_tag(name));
	return ret;
}

int Dimensions::parse(std::string data) {
	length = atoi(xml_parse_tag(data, "Length").c_str());
	width = atoi(xml_parse_tag(data, "Width").c_str());
	max_load_height = atoi(xml_parse_tag(data, "MaxLoadHeight").c_str());
	max_load_weight = atoi(xml_parse_tag(data, "MaxLoadWeight").c_str());
	return strlen(data.c_str());
}

std::string Dimensions::xml() {
	std::string ret("");
	ret.append(xml_start_tag("Dimensions"));
	ret.append(xml_make_tag("Length", itoa(length).c_str()));
	ret.append(xml_make_tag("Width", itoa(width).c_str()));
	ret.append(xml_make_tag("MaxLoadHeight", itoa(max_load_height).c_str()));
	ret.append(xml_make_tag("MaxLoadWeight", itoa(max_load_weight).c_str()));
	ret.append(xml_end_tag("Dimensions"));
	return ret;
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
	find_boundaries();
	return strlen(data.c_str());
}

std::string Package::xml() {
	std::string ret("");
	ret.append(xml_start_tag("Package"));
	ret.append(xml_make_tag("PackSequence", itoa(pack_sequence).c_str()));
	ret.append(article.xml());
	ret.append(barcode.xml());
	ret.append(place_position.xml("PlacePosition"));
	ret.append(xml_make_tag("Orientation", itoa(orientation).c_str()));
	ret.append(approach_point_1.xml("ApproachPoint1"));
	ret.append(approach_point_2.xml("ApproachPoint2"));
	ret.append(approach_point_3.xml("ApproachPoint3"));
	ret.append(xml_make_tag("StackHeightBefore", itoa(stack_height_before).c_str()));
	ret.append(xml_end_tag("Package"));
	return ret;
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

std::string PackPallet::xml() {
	std::string ret("");
	ret.append(xml_start_tag("PackPallet"));
	ret.append(xml_make_tag("PalletNumber", itoa(pallet_number).c_str()));
	ret.append(xml_make_tag("BruttoWeight", itoa(brutto_weight).c_str()));
	ret.append(xml_make_tag("NumberofPackages", itoa(n_package()).c_str()));
	ret.append(xml_make_tag("Description", description.c_str()));
	ret.append(dimension.xml());
	ret.append(xml_start_tag("Overhang"));
	ret.append(xml_make_tag("Length", "0"));
	ret.append(xml_make_tag("Width", "0"));
	ret.append(xml_end_tag("Overhang"));
	ret.append(xml_start_tag("Packages"));
	for(uint i = 0; i < n_package(); i++) {
		Package p = package[i];
		ret.append(p.xml());
	}
	ret.append(xml_end_tag("Packages"));
	ret.append(xml_end_tag("PackPallet"));
	return ret;
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

std::string PackList::xml() {
	std::string ret("");
	ret.append(xml_start_tag("PackList"));
	ret.append(xml_make_tag("OrderID", itoa(order_id).c_str()));
	ret.append(xml_start_tag("PackPallets"));
	for(uint i = 0; i < n_packpallet(); i++) {
		PackPallet p = packpallet[i];
		ret.append(p.xml());
	}
	ret.append(xml_end_tag("PackPallets"));
	ret.append(xml_end_tag("PackList"));
	return ret;
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
	ifs.close();
	return list;
}

void write_response(PackList list, const char* filename) {
	printf("Writing PackList to %s\n", filename);
	std::ofstream ofs(filename);
	std::string list_xml = list.xml();
	ofs.write(list_xml.c_str(), list_xml.size());
	ofs.close();
	printf("Writing Done.\n");
}
