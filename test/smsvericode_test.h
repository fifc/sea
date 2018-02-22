/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */
#ifndef _MIRR_SMS_VRFCODE_TESTCASE_H
#define _MIRR_SMS_VRFCODE_TESTCASE_H

#include <iostream>
#include <memory>

#include "sea.pb.h"
#include "testcase.h"

using namespace sea_proto;

class GetSmsVericodeTestCase: public TestCase {
 public:

  GetSmsVericodeTestCase(): TestCase("GetSmsVericode") { }

  std::string GenReq();

  void Report();
};

#endif // 
