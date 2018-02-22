/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */

#include <iostream>
#include <memory>

#include "getpicvericode_test.h"

std::string GetPicVericodeTestCase::GenReq() {
	GetPiCodeRequest request;
	request.mutable_client_info()->set_dev_key("thinkpad-12345");
	auto len = request.ByteSize();
	auto buf = new char[len + 1];
	request.SerializeToArray(buf, len);
	std::string data;
	data.assign(buf, len);
	delete[] buf;
	return data;
}

void GetPicVericodeTestCase::Report() {
	if (!got_resp_) {
		std::cout << __func__ << ": no respond\n";
	} else {
		GetPiCodeReply reply;
		std::cout << __func__ << ": len = " << len_ << "\n";
		if (!reply.ParseFromArray(data_, len_)) {
			std::cout << __func__ << ": invalid proto data received. len = " << len_ << "\n";
		} else {
			if (reply.err_info().err() == ErrorInfo::OK) {
				std::cout << "img data size: " << reply.img_data().size() << ", session: " << reply.session_id() << "\n";
			}
		}
	}
}
