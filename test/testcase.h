/*
 * http2 rpc test case
 *
 * Copyright (c) 2018 Steven Yi
 *
 */

#ifndef _SEA_TEST_CASE_H_
#define _SEA_TEST_CASE_H_

#include <iostream>
#include <memory>

//#include <nghttp2/asio_http2_client.h>
#include "sea.pb.h"
using namespace sea_proto;

class TestCase {
 protected:
  bool got_resp_ = {false};
  char *data_ = {nullptr};
  int len_ = {0};
  const std::string name_;

 public:
  TestCase(const char *name): name_(name) {}
  ~TestCase() { delete[] data_; }

  virtual void Report() = 0;
  virtual std::string GenReq() = 0;

  virtual void OnResponse() {
    got_resp_ = true;
  }

  virtual void OnData(const char *data, int len);
  virtual std::string GetUri();
};

#endif // _SEA_TEST_CASE_H_
