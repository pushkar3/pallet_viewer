/*
 * main.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include <stdio.h>
#include "xml_parser.h"
#include "packlist.h"
#include "order.h"

int main(int argc, char* argv[]) {

	OrderXML oxml;
	PackListXML pxml;

	oxml.parse("Example.order.xml", 1);
	pxml.parse("Example.packlist.xml", 1);

	return 0;
}
