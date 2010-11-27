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
  \file asBuilt.cc

  \brief Class structure to hold as built information for a mixed pallet.
  \code CVS Status:
  $Author: dr_steveb $
  $Revision: 1.2 $
  $Date: 2010/03/29 13:42:55 $
  \endcode

  \author Stephen Balakirsky
  \date March 23, 2010
*/

#ifdef MOASTSTATIC
#include "asBuilt.h"
#else
#include <asBuilt.h>
#endif
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>

class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string&s)
		: std::runtime_error(s)
	{ }
};

inline double BuiltPackage::convertToDouble( const std::string& s)
{
	std::istringstream i(s);
	double x;

	if( !(i >> x) )
		throw BadConversion("convertToDouble(\"" + s + "\")");
	return x;
}

inline int BuiltPackage::convertToInt( const std::string& s)
{
	std::istringstream i(s);
	int x;

	if( !(i >> x) )
		throw BadConversion("convertToInt(\"" + s + "\")");
	return x;
}

inline std::string BuiltPackage::parseString( const std::string & data, size_t &end )
{
	std::string searchString(",");
	size_t start = end+1;
	end = data.find(searchString,start);
	if( end == std::string::npos )
	{
		printf("asBuilt: error from parsing file\n");
		throw BadConversion("asBuilt: " + data);
	}
	return data.substr(start, end-start);
}

inline double BuiltPackage::parseDouble( const std::string & data, size_t &end )
{
	std::string searchString(",");
	size_t start = end+1;
	end = data.find(searchString,start);
	if( end == std::string::npos )
	{
		printf("asBuilt: error from parsing file\n");
		throw BadConversion("asBuilt: " + data);
	}
	return convertToDouble( data.substr(start, end-start) );
}

/*!
	Find the ending number in a string that is built as stringNumber,
	i.e. Box12 will return 12
*/
inline unsigned int BuiltPackage::parseEndNumber( const std::string & data )
{
	std::string searchString("0123456789");
	size_t found;
	found = data.find_first_of(searchString);
	if( found == std::string::npos )
	{
		printf("asBuilt: error from parsing file\n");
		throw BadConversion("asBuilt: " + data);
	}
	return convertToInt( data.substr(found) );
}

inline Article BuiltPackage::findArticle( int articleID, const Order &order )
{
	for( unsigned int i=0; i< order.orderline.size(); i++ )
	{
	  if( order.orderline[i].article.id == (unsigned int)articleID )
			return order.orderline[i].article;
	}
	throw BadConversion("asBuilt: Unable to find article id: " + articleID );
}

BuiltPackage BuiltPackage::parse(std::string data, const Order &order)
{
	BuiltPackage retValue;
	size_t found;
	std::string searchString(",");

	// add a final ',' on the end to allow easy parsing
	data.append(",");

	// parse name
	found = -1;
	retValue.name = parseString(data, found);
	retValue.id = parseEndNumber(retValue.name);

	// parse package class
	retValue.packageClass = parseString(data, found);
	retValue.type = parseEndNumber(retValue.packageClass);

	// parse memory
	retValue.memory = parseString(data, found);

	// parse x, y, z
	retValue.x = parseDouble(data, found);
	retValue.y = parseDouble(data, found);
	retValue.z = parseDouble(data, found);

	// parse roll, pitch, yaw
	retValue.roll = parseDouble(data, found);
	retValue.pitch = parseDouble(data, found);
	retValue.yaw = parseDouble(data, found);

	// set article information
	retValue.article = findArticle(retValue.type, order);

	if(debug)
		printf( "found line: <%s:%d> <%s:%d> <%s> <%f %f %f> <%f %f %f> article %d: <%d %d>\n", 
			retValue.name.c_str(), retValue.id,
			retValue.packageClass.c_str(), retValue.type, retValue.memory.c_str(),
			retValue.x, retValue.y, retValue.z,
			retValue.roll, retValue.pitch, retValue.yaw,
			retValue.article.id, retValue.article.length, retValue.article.width);

	return retValue;
}

int AsBuilt::parse(std::ifstream &ifs, const Order &order)
{
	char* line = (char*) malloc (255);

	packages.clear();

	ifs.getline(line,255);
	if(debug) {
		printf("asBuilt::Read line: %s\n", line );
	}
	BuiltPackage builtPackage(debug);
	while( ifs.getline(line,255) )
	{
		packages.push_back(builtPackage.parse(line, order));
	}
	printf("AsBuilt Read %d packages.\n", (int)packages.size());
	free(line);
	return 1;
}


AsBuilt read_asBuilt(const char* filename, Order order, int debug) {
	std::ifstream ifs(filename);
	AsBuilt asBuilt(debug);

	if(!ifs.is_open()) {
		printf("%s File not found.\n", filename);
		asBuilt.packages.clear();
		return(asBuilt);
	}
	asBuilt.parse(ifs, order);
	return asBuilt;
}
