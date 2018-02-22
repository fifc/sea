/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2015 Tatsuhiro Tsujikawa
 *
 */

#ifndef _MIRR_GETPICVERICODE_H
#define _MIRR_GETPICVERICODE_H

#include <iostream>
#include <memory>

#include "sea.pb.h"
#include "testcase.h"

using namespace sea_proto;

class GetPicVericodeTestCase: public TestCase {
 public:

  GetPicVericodeTestCase(): TestCase("GetPicVericode") { }
  std::string GenReq();
  void Report();
};
#endif //
