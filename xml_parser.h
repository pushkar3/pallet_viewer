/*
 * xml_parser.h
 *
 *  Created on: Mar 4, 2010
 *      Author: pushkar
 */

#ifndef XML_PARSER_H_
#define XML_PARSER_H_

#include <iostream>
#include <sstream>

std::string itoa(int n);

// Find file size
unsigned int xml_parser_get_buffer_length(const char* filename);

// Make XML tag value pair and return as a string
std::string xml_make_tag(const char* tag, const char* value);

std::string xml_start_tag(const char* tag);
std::string xml_value_tag(const char* value);
std::string xml_end_tag(const char* tag);

// Deprecated: Parse XML between start and end tokens
std::string xml_parse(std::string text, const char* start, const char* end);

// Parse XML file for any tag and return data within it
std::string xml_parse_tag(std::string text, const char* tag);

// Find the position of the first start tag in the XML file
int xml_find_tag_start(std::string text, const char* tag);

// Find the position of the first end tag in the XML file
int xml_find_tag_end(std::string text, const char* tag);

// Remove first occurence of XML data tagged with token tag
int xml_parse_remove_first_tag(std::string *text, const char* tag);

#endif /* XML_PARSER_H_ */
