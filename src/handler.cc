/*
 *
 * Copyright (c) 2016 Sea Co. Ltd. all rights reserved
 *
 * Steven Yi
 * 2015-12
 *
 */

#include <iostream>
#include "handler.h"
#include "db_mgr.h"
#include "picode.h"
#include "cfg.h"
#include "sms/send_sms.h"

SignupReply SeaHandler::Signup(const SignupRequest& req) {
  SignupReply reply;
  auto err_info = reply.mutable_err_info();
  err_info->set_err(ErrorInfo::ERROR);
  do {
    int64_t uid = -1;
    if (!db_mgr_->GetUidByPhone(req.phone(), &uid)) {
      err_info->set_msg("error verify phone");
      break;
    }
    if (uid >= 0) {
      err_info->set_msg("phone already registered");
      break;
    }
    if (!db_mgr_->VerifySmsVericode(req.session_id(), req.phone(), req.verify_code())) {
      err_info->set_msg("verify code is incorrect");
      break;
    }
    uid = db_mgr_->CreateUserAccount(req);
    if (uid < 0) {
      err_info->set_msg("error create account");
      break;
    }
    if (req.passwd().size() < 3) {
      err_info->set_msg("password too short");
      break;
    }
    reply.set_uid(uid);
    err_info->set_err(ErrorInfo::OK);
  } while (0);
  return reply;
}

GetPiCodeReply SeaHandler::GetPiCode(const GetPiCodeRequest& req) {
  VLOG(60) << __func__ << " req -> " << req.DebugString();
  GetPiCodeReply reply;
  do {
    auto err_info = reply.mutable_err_info();
    err_info->set_err(ErrorInfo::ERROR);

    if (!req.has_client_info() || req.client_info().dev_key().empty()) {
      err_info->set_msg("device key not specified");
      break;
    }
    std::string session_id = req.client_info().dev_key();
    session_id += '-';
    session_id += std::to_string(time(0));

    std::string img_data;
    auto verify_code = CreatePicVericode(&img_data);
    if (verify_code.empty()) break;
  
    bool ok = db_mgr_->SavePicVericode(session_id, verify_code);
    if (!ok) {
      err_info->set_msg("db error");
    } else {
      reply.set_session_id(session_id);
      reply.set_img_data(img_data.c_str(), img_data.size());
      reply.clear_err_info();
    }
  } while (0);

  VLOG(60) << __func__ << " reply <- " << reply.Utf8DebugString();
  return reply;
}

static std::string GetDialablePhone(const std::string& phone) {
  if (phone.empty()) return "";
  if (phone[0] == '+') return "";
  auto pos = phone.find('-');
  if (pos == std::string::npos || pos == 0) return "";
  auto dialable = "+" + phone.substr(0, pos) + phone.substr(pos + 1);
  return dialable;
}

GetSmsVericodeReply SeaHandler::GetSmsVericode(const GetSmsVericodeRequest& req) {
  GetSmsVericodeReply reply;
  auto err_info = reply.mutable_err_info();
  err_info->set_err(ErrorInfo::ERROR);

  do {
    if (req.session_id().empty()) {
      err_info->set_msg("invalid session");
      break;
    }
    if (req.pic_code().empty()) {
      err_info->set_msg("empty verify code");
      break;
    }
    if (!db_mgr_->VerifyPicVericode(req.session_id(), req.pic_code())) {
      err_info->set_msg("invalid verify code");
      break;
    }
    auto dial_phone = GetDialablePhone(req.phone());
    if (dial_phone.empty()) {
      err_info->set_msg("invalid phone number: " + req.phone() + ". should in format like 86-18812345678");
      break;
    }
    int64_t uid = -1;
    if (!db_mgr_->GetUidByPhone(req.phone(), &uid)) {
      err_info->set_msg("error verify phone");
      break;
    }
    if (uid >= 0) {
      err_info->set_msg("phone already registered");
      break;
    }
    int code = drand48() * 10000;
    char buf[10];
    sprintf(buf, "%04d", code);
    if (!db_mgr_->SaveSmsVericode(req.session_id(), buf)) {
      err_info->set_msg("error generate verify code");
      break;
    }
    if (!db_mgr_->SaveSessionPhone(req.session_id(), req.phone())) {
      err_info->set_msg("error generate verify code");
      break;
    }
    if (sms::SendSM(dial_phone, std::string("您的验证码是") + buf)) {
      err_info->set_msg("error send verify code");
      break;
    }
    err_info->set_err(ErrorInfo::OK);
    reply.set_expire_time(120);
  } while (0);
  return reply;
}

LoginReply SeaHandler::Login(const LoginRequest& req) {
  VLOG(60) << __func__ << " req -> " << req.DebugString();
  std::cout << __func__ << " req -> " << req.DebugString() << "\n";
  LoginReply reply;

  do {
    auto err_info = reply.mutable_err_info();
    err_info->set_err(ErrorInfo::ERROR);
    if (GetDialablePhone(req.phone()).empty()) {
      err_info->set_msg("phone number is invalid: " + req.phone() + ". should in format like 86-18812345678");
      break;
    }
    if (!req.has_client_info() || req.client_info().dev_key().empty()) {
      err_info->set_msg("device key is required");
      break;
    }
  
    auto user_info = reply.mutable_user_info();
    bool ok = db_mgr_->GetUserBasicInfo(req.phone().c_str(), user_info);
    if (!ok)  {
      reply.clear_user_info();
      err_info->set_msg("db error");
      break;
    }
    if (user_info->uid() == 0 && user_info->phone().empty()) {
      reply.clear_user_info();
      err_info->set_msg("user not exists");
      break;
    }
    if (req.passwd() != user_info->passwd()) {
        reply.clear_user_info();
        err_info->set_msg("auth failure");
    	err_info->set_err(ErrorInfo::AUTH_FAIL);
	break;
    }
    auto session_str = db_mgr_->CreateLoginSession(user_info->uid(),req.client_info().dev_key());
    if (session_str.empty()) {
        reply.clear_user_info();
        err_info->set_msg("error create session");
        break;
    }
    reply.clear_err_info();
    user_info->clear_passwd();
    reply.set_session(session_str);
  } while (0);

  VLOG(60) << __func__ << " reply <- " << reply.DebugString();
  return reply;
}

ErrorInfo SeaHandler::Logout(const LogoutRequest& req) {
  ErrorInfo reply;
  return reply;
}

template <typename T>
int Json2Pb(const std::string& json, T* pb) {
	return -2;
}

template <typename T>
int Pb2Json(const T& pb, std::string *json) {
	return -2;
}

#define ADD_PROTO_HANDLER(name) handler_map_[#name] = \
	[this](const http_req &req, const http_res &res) { \
		auto data = new std::string(); \
		req.on_data([this, &req, &res, data](const uint8_t *buf, std::size_t len) { \
			if (len > 0) { \
				data->append((char *)buf, len); \
				return; \
			} \
			if (data->empty()) { \
				delete data; \
      				res.write_head(400); \
      				res.end("empty request\n"); \
				return; \
			} \
			name ## Request request; \
			if (!request.ParseFromArray(data->c_str(), data->size())) { \
				delete data; \
      				res.write_head(401); \
      				res.end("invalid post payload protobuf\n"); \
				return; \
			} \
			delete data; \
			auto reply = this->name(request); \
      			res.write_head(200, {{"sea", {"ok!"}}}); \
			auto res_len = reply.ByteSize(); \
			auto res_buf = new char[res_len + 1]; \
			if (!reply.SerializeToArray(res_buf, res_len)) { \
      				res.write_head(500); \
      				res.end("internal error\n"); \
				delete[] res_buf; \
				return; \
			} \
			std::string payload; \
			payload.assign(res_buf, res_len); \
      			res.end(payload); \
			delete[] res_buf; \
		}); \
	} 

#define ADD_JSON_HANDLER(name) json_handler_map_[#name] = \
	[this](const http_req &req, const http_res &res) { \
	    auto data = new std::string(); \
	    req.on_data([this, &req, &res, data](const uint8_t *buf, std::size_t len) { \
		if (len > 0) { \
			data->append((char *)buf, len); \
			return; \
		} \
		if (data->empty()) { \
			delete data; \
      			res.write_head(400); \
      			res.end("empty request\n"); \
			return; \
		} \
		name ## Request request; \
		auto ret = Json2Pb(*data, &request); \
		delete data; \
		if (ret != 0) { \
			if (ret == -1) { \
      				res.write_head(401); \
      				res.end("invalid post payload json\n"); \
			} else { \
      				res.write_head(402); \
      				res.end("not implenmented\n"); \
			} \
			return; \
		} \
		auto reply = this->name(request); \
      		res.write_head(200, {{"sea", {"ok!"}}}); \
		std::string json; \
		if (Pb2Json(reply, &json) != 0) { \
      			res.write_head(500); \
      			res.end("internal error\n"); \
			return; \
		} \
      		res.end(json); \
	    }); \
	}

#define ADD_HANDLER(name) \
	ADD_PROTO_HANDLER(name); \
	ADD_JSON_HANDLER(name)

SeaHandler::SeaHandler() {
  ADD_HANDLER(Login);
  ADD_HANDLER(Logout);
  ADD_HANDLER(Signup);
  ADD_HANDLER(GetPiCode);
  ADD_HANDLER(GetSmsVericode);

  db_mgr_ = std::make_shared<DBMgr>(FLAGS_db_cfg.c_str());
}
