
#include "uid.h"
#include "iset.h"
#include <iostream>

static iset *g_iset = NULL;

std::string GetUid(const std::string& query) {
	if (g_iset == NULL) {
		g_iset = new iset;
		iset_init(g_iset);
		iset_add(g_iset, 1000000, 10000000);
	}
	auto p0 = query.find("query");
	if (p0 != std::string::npos) {
		std::string str;
		iset_map(g_iset, [](unsigned long* p, void *data) {
			*(std::string*)data += '[' + std::to_string(p[0]) + ',' + std::to_string(p[1]) + ") ";
			return 0;
		}, &str);
		return str;
	}
	p0 = query.find("s=");
	if (p0 != std::string::npos) {
		std::string str;
		p0 += 2;
		auto p1 = query.find('&', p0);
		if (p1 != std::string::npos)
			str = query.substr(p0,p1-p0);
		else
			str = query.substr(p0);
		if (!str.empty()) {
			auto base = strtoul(str.c_str(), NULL, 10);
			auto uid = iset_start(g_iset, base);
			return std::to_string(uid);
		}
	}
	auto uid = iset_get(g_iset);
	return std::to_string(uid);
}
