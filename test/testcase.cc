/*
 * http2 testcase 
 *
 * Copyright (c) 2018 Steven Yi
 *
 */

#include <iostream>
#include <memory>

#include "testcase.h"

using namespace sea_proto;

static std::string schema = "http://localhost/akago/";

void TestCase::OnData(const char *data, int len) {
	if (len != 0) {
		auto buf = new char[len + len_ + 1];
		if (data_ != nullptr) {
			memmove(buf, data_, len_);
			delete[] data_;
		}
		memmove(buf + len_, data, len);
		data_ = buf;
		len_ += len;
		data_[len_] = 0;
	}
}

std::string TestCase::GetUri() {
	return schema + name_;
}

