/*
 *
 * Copyright (c) 2018 Sea Co. Ltd. all rights reserved
 *
 * Steven Yi
 * 2018-02
 *
 */

#ifndef SEA_HTTP_HANDLER_H
#define SEA_HTTP_HANDLER_H

#include <map>
#include <functional>
#include <string>

#include <nghttp2/asio_http2_server.h>
#include "sea.pb.h"

using http_req = nghttp2::asio_http2::server::request;
using http_res = nghttp2::asio_http2::server::response;
using namespace sea_proto;

class DBMgr;
class SeaHandler {
 public:
  SeaHandler();
  using http_hander = std::function<void(const http_req&, const http_res&)>;
  std::map<std::string, http_hander> handler_map_;
  std::map<std::string, http_hander> json_handler_map_;

 public:
  LoginReply Login(const LoginRequest& req);
  ErrorInfo Logout(const LogoutRequest& req);
  SignupReply Signup(const SignupRequest& req);
  GetPiCodeReply GetPiCode(const GetPiCodeRequest&);
  GetSmsVericodeReply GetSmsVericode(const GetSmsVericodeRequest&);

 private:
  std::shared_ptr<DBMgr> db_mgr_;
};

#endif // SEA_HTTP_HANDLER_H
