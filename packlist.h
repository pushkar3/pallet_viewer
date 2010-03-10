/*
 * Class structure to parse input orders for a mixed pallet.
 * KUKA input file definition for orders is used here.
 *
 * Author: Pushkar Kolhe
*/

#ifndef PACKLIST_H_
#define PACKLIST_H_

#include <vector>
#include <string.h>
#include "xml_parser.h"

class Article {
public:
	unsigned int id;
	std::string description;
	unsigned int type;
	unsigned int length;
	unsigned int width;
	unsigned int height;
	unsigned int weight;
	unsigned int family;

	Article() {}
	~Article() {}
	int parse(std::string data);
};

class Barcode {
public:
	std::string code;

	Barcode() {}
	~Barcode() {}
	int parse(std::string data);
};

class OrderLine {
public:
	unsigned int orderlineno;
	Article article;
	std::vector<Barcode> barcode;

	OrderLine() {}
	~OrderLine() {}
	unsigned n_barcode() { return barcode.size(); }
	int parse(std::string data);
};

class Restrictions {
public:
	bool familygrouping;
	bool ranking;

	Restrictions() {
		familygrouping = false;
		ranking = false;
	}
	~Restrictions() {}
	int parse(std::string data);
};

class Order {
public:
	unsigned int id;
	std::string description;
	Restrictions restriction;
	std::vector <OrderLine> orderline;

	Order() {}
	~Order() {}
	unsigned int n_orderline() { return orderline.size(); }
	int parse(std::string data);
};

Order read_order(const char* filename, int debug);

#endif
