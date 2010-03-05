/*
 * response.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include "response.h"
#include "xml_parser.h"
#include <stdlib.h>

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
