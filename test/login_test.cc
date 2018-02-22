/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */

#include <iostream>
#include <memory>

#include "login_test.h"

std::string LoginTestCase::GenReq() {
	LoginRequest request;
	request.set_phone("86-13800010001");
	request.set_passwd("mirr123");
	auto client_info = request.mutable_client_info();
	client_info->set_dev_key("http2-cli-login");
	auto len = request.ByteSize();
	auto buf = new char[len + 1];
	request.SerializeToArray(buf, len);
	std::string data;
	data.assign(buf, len);
	delete[] buf;
	return data;
}

void LoginTestCase::Report() {
	if (!got_resp_) {
		std::cout << __func__ << ": no respond\n";
	} else {
		LoginReply reply;
		std::cout << __func__ << ": len = " << len_ << "\n";
		if (!reply.ParseFromArray(data_, len_)) {
			std::cout << __func__ << ": invalid proto data received. len = " << len_ << "\n";
		} else {
			std::cout << reply.ShortDebugString() << "\n";
			if (reply.err_info().err() == ErrorInfo::OK) {
			}
		}
	}
}
