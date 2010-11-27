/*
  DISCLAIMER:
  This software was produced by the National Institute of Standards
  and Technology (NIST), an agency of the U.S. government, and by statute is
  not subject to copyright in the United States.  Recipients of this software
  assume all responsibility associated with its operation, modification,
  maintenance, and subsequent redistribution.

  See NIST Administration Manual 4.09.07 b and Appendix I. 
*/

/*!
  \file asBuilt.h

  \brief Class structure to hold as built information for a mixed pallet.
  \code CVS Status:
  $Author: dr_steveb $
  $Revision: 1.1 $
  $Date: 2010/03/29 13:22:40 $
  \endcode

  \author Stephen Balakirsky
  \date March 23, 2010
*/

#ifndef ASBUILT_H_
#define ASBUILT_H_

#include <vector>
#include <string.h>
#include "packlist.h"

class BuiltPackage {
public:
	std::string name;
	unsigned int id;
	std::string packageClass;
	unsigned int type;
	std::string memory;
	double x, y, z;
	double roll, pitch, yaw;
	Article article;
	int debug;

	Article findArticle( int articleID, const Order &order );
	double convertToDouble(const std::string & s);
	int convertToInt(const std::string & s);
	std::string parseString( const std::string & data, size_t &end );
	double parseDouble( const std::string & data, size_t &end );
	unsigned int parseEndNumber( const std::string & data );
	BuiltPackage(int debugIn=0) {debug = debugIn;}
	~BuiltPackage() {}
	BuiltPackage parse(std::string data, const Order &order);
};

class AsBuilt {
public:
	int debug;
	unsigned int palletID;
	std::string description;
	std::vector <BuiltPackage> packages;

	AsBuilt(int debugIn=0) {debug = debugIn;}
	~AsBuilt() {}
	unsigned int n_packages() { return packages.size(); }
	int parse(std::ifstream &ifs, const Order &order);
};

AsBuilt read_asBuilt(const char* filename, Order order, int debug);

#endif
