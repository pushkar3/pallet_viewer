/*

 * xml_parser.cc

 *

 *  Created on: Mar 4, 2010

 *      Author: pushkar

 *		Brief:	Parses XML files. Parses XML within given tags. 

 *				It can automatically determined XML buffer size, detect start and end tags.

 */
#ifdef WIN32
#define snprintf sprintf_s
//#define strncpy strncpy_s
#endif

#include "xml_parser.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>

// Find file size

unsigned int xml_parser_get_buffer_length(const char* filename) {

	std::ifstream is;

	is.open(filename);

	if(!is.is_open()) return 0;



	is.seekg(0, std::ios::end);

	unsigned int ret = (unsigned int)is.tellg();

	is.close();

	return ret;

}



// Deprecated: Parse XML between start and end tokens

std::string xml_parse(std::string text, const char* start, const char* end) {

	std::string ret("");

	int i_start = text.find(start) + strlen(start);

	int i_end = text.find(end);

	if(i_start > 0 && i_end > 0 && i_end > i_start) {

		ret = text.substr(i_start, i_end-i_start);

	}

	return ret;

}



// Parse XML file for any tag and return data within it

std::string xml_parse_tag(std::string text, const char* tag) {

	std::string ret("");

	int i_tag = strlen(tag);

	char* start = (char*) malloc(sizeof(tag) + 25);

	char* end = (char*) malloc(sizeof(tag) + 25);

	snprintf(start, sizeof(tag) + 25, "<%s>", tag);

	snprintf(end, sizeof(tag) + 25, "</%s>", tag);

	int i_start = text.find(start);

	int i_end = text.find(end);

	i_tag = strlen(start);



	if(i_start < 0) {

		memset(start, '\0', sizeof(start));

		snprintf(start, sizeof(tag) + 25, "<%s ", tag);

		i_start = text.find(start);

		if(i_start > -1) {

			std::string tmp = text.substr(i_start, i_end-i_start);

			i_tag = tmp.find(">")+1;

		}

	}



	if(i_start > -1 && i_end > -1 && i_end > i_start) {

		i_start += i_tag;

		ret = text.substr(i_start, i_end-i_start);

	}



	return ret;

}



// Find the position of the first start tag in the XML file

int xml_find_tag_start(std::string text, const char* tag) {

	char* start = (char*) malloc(sizeof(tag) + 25);

	snprintf(start, sizeof(tag) + 25, "<%s>", tag);

	int i_start = text.find(start);



	if(i_start < 0) {

		memset(start, '\0', sizeof(start));

		snprintf(start, sizeof(tag) + 25, "<%s ", tag);

		i_start = text.find(start);

	}



	return i_start;

}



// Find the position of the first end tag in the XML file

int xml_find_tag_end(std::string text, const char* tag) {

	char* end = (char*) malloc(sizeof(tag) + 25);

	snprintf(end, sizeof(tag) + 25, "</%s>", tag);

	int i_end = text.find(end)+strlen(end);

	return i_end;

}



// Remove first occurence of XML data tagged with token tag

int xml_parse_remove_first_tag(std::string *text, const char* tag) {

	int i_start = 0;

	int i_end = 0;

	i_start = xml_find_tag_start(*text, tag);

	i_end = xml_find_tag_end(*text, tag);

	if(i_start > -1 && i_end > -1 && i_end > i_start) {

		(*text).erase(i_start, i_end-i_start);

		return 1;

	}

	return 0;

}

