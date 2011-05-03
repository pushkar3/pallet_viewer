/*
 * xml_parser.cpp
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#include "xml_parser.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>

static int depth = 0;

std::string itoa(int n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
}

// Find file size
unsigned int xml_parser_get_buffer_length(const char* filename) {
	std::ifstream is;
	is.open(filename);
	if(!is.is_open()) return 0;

	is.seekg(0, std::ios::end);
	unsigned int ret = is.tellg();
	is.close();
	return ret;
}

// Make XML tag value pair and return as a string
std::string xml_make_tag(const char* tag, const char* value) {
	std::string ret("");
	for(int i = 0; i < depth; i++) ret.append("\t");
	ret.append("<");
	ret.append(tag);
	ret.append(">");
	ret.append(value);
	ret.append("</");
	ret.append(tag);
	ret.append(">");
	ret.append("\n");
	return ret;
}

std::string xml_start_tag(const char* tag) {
	std::string ret("");
	for(int i = 0; i < depth; i++) ret.append("\t");
	ret.append("<");
	ret.append(tag);
	ret.append(">");
	ret.append("\n");
	depth++;
	return ret;
}

std::string xml_value_tag(const char* value) {
	std::string ret("");
	for(int i = 0; i < depth; i++) ret.append("\t");
	ret.append(value);
	ret.append("\n");
	return ret;
}

std::string xml_end_tag(const char* tag) {
	std::string ret("");
	depth--;
	for(int i = 0; i < depth; i++) ret.append("\t");
	ret.append("</");
	ret.append(tag);
	ret.append(">");
	ret.append("\n");
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
	sprintf(start, "<%s>", tag);
	sprintf(end, "</%s>", tag);
	int i_start = text.find(start);
	int i_end = text.find(end);
	i_tag = strlen(start);

	if(i_start < 0) {
		memset(start, '\0', sizeof(start));
		sprintf(start, "<%s ", tag);
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
	sprintf(start, "<%s>", tag);
	int i_start = text.find(start);

	if(i_start < 0) {
		memset(start, '\0', sizeof(start));
		sprintf(start, "<%s ", tag);
		i_start = text.find(start);
	}

	return i_start;
}

// Find the position of the first end tag in the XML file
int xml_find_tag_end(std::string text, const char* tag) {
	char* end = (char*) malloc(sizeof(tag) + 25);
	sprintf(end, "</%s>", tag);
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
