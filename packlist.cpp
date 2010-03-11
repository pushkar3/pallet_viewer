/*
 * packlist.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include "packlist.h"
#include "xml_parser.h"
#include <stdlib.h>
#include <fstream>

int Article::parse(std::string data) {
	id = atoi(xml_parse_tag(data, "ID").c_str());
	type = atoi(xml_parse_tag(data, "Type").c_str());
	length = atoi(xml_parse_tag(data, "Length").c_str());
	width = atoi(xml_parse_tag(data, "Width").c_str());
	height = atoi(xml_parse_tag(data, "Height").c_str());
	family = atoi(xml_parse_tag(data, "Family").c_str());
	weight = atoi(xml_parse_tag(data, "Weight").c_str());
	description = xml_parse_tag(data, "Description");
	return strlen(data.c_str());
}

int Barcode::parse(std::string data) {
	code = data;
	return strlen(data.c_str());
}

int OrderLine::parse(std::string data) {
	orderlineno = atoi(xml_parse_tag(data, "OrderLineNo").c_str());
	std::string s_article = xml_parse_tag(data, "Article");
	article.parse(s_article);
	int ret = 1;
	while(ret) {
		std::string s_barcode = xml_parse_tag(data, "Barcode");
		if(strlen(s_barcode.c_str()) > 0) {
			Barcode b;
			b.parse(s_barcode);
			barcode.push_back(b);
		}
		ret = xml_parse_remove_first_tag(&data, "Barcode");
	}
	return strlen(data.c_str());
}

int Restrictions::parse(std::string data) {
	if(strcmp(xml_parse_tag(data, "FamilyGrouping").c_str(), "True") == 0)
		familygrouping = true;
	if(strcmp(xml_parse_tag(data, "Ranking").c_str(), "True") == 0)
		ranking = true;
	return strlen(data.c_str());
}

int Order::parse(std::string data) {
	int ret = 1;

	id = atoi(xml_parse_tag(data, "ID").c_str());
	description = xml_parse_tag(data, "Description");
	restriction.parse(xml_parse_tag(data, "Restrictions"));

	while(ret) {
		std::string s_order = xml_parse_tag(data, "OrderLine");
		if(strlen(s_order.c_str()) > 0) {
			OrderLine o;
			o.parse(s_order);
			orderline.push_back(o);
		}
		ret = xml_parse_remove_first_tag(&data, "OrderLine");
	}

	return (ret && strlen(data.c_str()));
}

Order read_order(const char* filename, int debug) {
	std::ifstream ifs(filename);
	if(!ifs.is_open()) {
		printf("%s File not found.\n Exiting.\n", filename);
		exit(1);
	}

	int buf_len = xml_parser_get_buffer_length(filename);
	char* orderlist_buf = (char*) malloc (buf_len + 1);
	ifs.read(orderlist_buf, buf_len);
	std::string orderlist_xml = orderlist_buf;

	Order order;
	order.parse(xml_parse_tag(orderlist_xml, "Order"));

	if(debug) {
		printf("Number of Orders: %d\n", order.n_orderline());

		for(uint i = 0; i < order.n_orderline(); i++) {
			printf("%d. OrderLine\n", i);
			printf("  Article Id:\t %d\n", order.orderline[i].article.id);
			printf("  Description:\t %s\n", order.orderline[i].article.description.c_str());
			printf("  Size:  \t <%d, %d, %d>\n", order.orderline[i].article.width,
					order.orderline[i].article.length,
					order.orderline[i].article.height);
			printf("  Weight:\t %d\n", order.orderline[i].article.weight);
			printf("  Barcodes:\n");
			for(uint j = 0; j < order.orderline[i].n_barcode(); j++) {
				printf("\t\t %s\n", order.orderline[i].barcode[j].code.c_str());
			}
		}
	}

	printf("Order Read.\n");
	free(orderlist_buf);
	return order;
}
