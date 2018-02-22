/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */

#include <iostream>
#include <memory>

#include "smsvericode_test.h"

std::string GetSmsVericodeTestCase::GenReq() {
	GetSmsVericodeRequest request;
	request.set_phone("86-18603000506");
	request.set_pic_code("flSwC1");
	request.set_session_id("thinkpad-12345-1451569012");
	auto client_info = request.mutable_client_info();
	client_info->set_dev_key("http2-cli-signup");
	auto len = request.ByteSize();
	auto buf = new char[len + 1];
	request.SerializeToArray(buf, len);
	std::string data;
	data.assign(buf, len);
	delete[] buf;
	return data;
}

void GetSmsVericodeTestCase::Report() {
	if (!got_resp_) {
		std::cout << __func__ << ": no respond\n";
		return;
	}
	GetSmsVericodeReply reply;
	std::cout << __func__ << ": len = " << len_ << "\n";
	if (!reply.ParseFromArray(data_, len_)) {
		std::cout << __func__ << ": invalid proto data received. len = " << len_ << "\n";
		return;
	}
	std::cout << reply.DebugString() << "\n";
	if (reply.err_info().err() == ErrorInfo::OK) {
	}
}
