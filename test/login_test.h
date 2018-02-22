/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */
#ifndef _SEA_LOGIN_TESTCASE_H
#define _SEA_LOGIN_TESTCASE_H

#include <iostream>
#include <memory>

#include "sea.pb.h"
#include "testcase.h"

using namespace sea_proto;

class LoginTestCase: public TestCase {
 public:

  LoginTestCase(): TestCase("Login") { }

  std::string GenReq();

  void Report();
};

#endif // 
