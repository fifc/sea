/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */

#include <iostream>
#include <memory>

#include "html_test.h"

std::string HtmlTestCase::GenReq() {
	return "";
}

void HtmlTestCase::Report() {
	if (!got_resp_) {
		std::cout << __func__ << ": no respond\n";
	} else {
		std::cout << __func__ << ": len = " << len_ << ", data =>\n";
		std::cout << data_ << "\n";
	}
}
