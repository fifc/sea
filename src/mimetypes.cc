/*
 *
 * Copyright (c) 2018 Sea Co. Ltd. all rights reserved
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <map>
#include <strings.h>

class StringCaseCompare {
	public:
		bool operator()(const std::string& a, const std::string&b) 
		{
			return strcasecmp(a.c_str(), b.c_str()) < 0;
		}
};

static std::map<std::string, std::string, StringCaseCompare> g_mimemap = {
	{ "html", "text/html"},
	{ "htm", "text/html"},
	{ "js", "application/javascript"},
	{ "txt", "text/plain"},
	{ "cc", "text/plain"},
	{ "h", "text/plain"},
	{ "jpg", "image/jpeg"},
	{ "png", "image/png"},
	{ "svg", "image/svg"},
	{ "json", "application/json"},
	{ "xml", "application/xml"},
	{ "proto", "application/json"},
};

std::string get_mimetype(const std::string& path)
{
        auto pos = path.find_last_of(".");
        if (pos == std::string::npos)
                return "";
        auto ext = path.substr(pos+1);
        auto iter = g_mimemap.find(ext);
        if (iter != g_mimemap.end())
                return iter->second;
        return "";
}
