/*
 *
 * Copyright (c) 2018 Sea Co. Ltd. all rights reserved
 *
 * Steven Yi
 * 2018-02
 *
 */

#ifndef _SEA_DB_MGR_H
#define _SEA_DB_MGR_H

#include <map>
#include <string>
#include <mutex>
#include <functional>
#include <libpq-fe.h>

#include "user.pb.h"
#include "sea.pb.h"

using namespace sea_proto;
using namespace user_proto;

class DBMgr {
 public:
  DBMgr(const char *conn_str);

  ~DBMgr();

  bool GetUserBasicInfo(const char *phone, UserBasicInfo *out) {
    return GetUserBasicInfo(0, phone, nullptr, out);
  }

  bool GetUserBasicInfoByOpenId(const char *openid, UserBasicInfo *out) {
    return GetUserBasicInfo(0, nullptr, openid, out);
  }

  std::string GetPicVericode(const std::string&);
  std::string GetSmsVericode(const std::string&);

  bool GetUidByPhone(const std::string& phone, int64_t *uid);
  bool SavePicVericode(const std::string& session, const std::string& code);
  bool SaveSmsVericode(const std::string& session, const std::string& code);
  bool SaveSessionPhone(const std::string& session, const std::string& phone);
  bool VerifyPicVericode(const std::string& session, const std::string& code);
  bool VerifySmsVericode(const std::string& session,
    const std::string& phone, const std::string& code);
  std::string CreateLoginSession(int64_t uid, const std::string& device_key);
  int64_t CreateUserAccount(const SignupRequest& req);
  int64_t CreateUserAccount(const std::string& id_type, const std::string& openid,
    const std::string& name, const std::string& passwd);

 private:
  bool GetUserBasicInfo(int64_t uid, const char *phone, const char *openid, UserBasicInfo *out);
  int64_t GetUidByOpenId(const char *openid);

 private:
  PGconn *pg_conn_;
  std::mutex uid_mutex_;
};

#endif // _SEA_DB_MGR_H
