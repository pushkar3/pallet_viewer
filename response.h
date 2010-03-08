/*
 * Class structure to parse output pallet configuration for a mixed pallet.
 * KUKA output file definition for orders is used here.
 *
 * Author: Pushkar Kolhe
*/

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include "packlist.h"
#include <vector>

class Point {
public:
	int x;
	int y;
	unsigned int z;

	Point() {}
	~Point() {}
	int parse(std::string data);
};

class Package {
public:
	unsigned int pack_sequence;
	unsigned int incoming_sequence;
	unsigned int orderlineno;
	unsigned int parent_layer;
	Article article;
	Barcode barcode;
	Point place_position;
	unsigned int orientation;
	Point approach_point_1;
	Point approach_point_2;
	Point approach_point_3;
	unsigned int stack_height_before;

	Package() {};
	~Package() {};
	int parse(std::string data);
};

class Dimensions {
public:
	unsigned int length;
	unsigned int width;
	unsigned int max_load_weight;
	unsigned int max_load_height;

	Dimensions() {}
	~Dimensions() {}
	int parse(std::string data);
};

class PackPallet {
public:
	unsigned int pallet_number;
	unsigned int brutto_weight;
	unsigned int number_of_packages;
	std::string description;
	Dimensions dimension;
	std::vector<Package> package;

	PackPallet() {};
	~PackPallet() {};
	unsigned int n_package() { return package.size(); }
	int parse(std::string data);
};

class PackList {
public:
	unsigned int order_id;
	std::vector<PackPallet> packpallet;

	PackList() {};
	~PackList() {};
	unsigned int n_packpallet() { return packpallet.size(); }
	int parse(std::string data);
};

#endif /* RESPONSE_H_ */