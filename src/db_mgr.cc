/*
 * db_mgr.c
 * database interface
 * Steven Yi
 * Copyright (C) 2018, Sea Co. Ltd. All rights reserved 
 */

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include <iostream>

#include "db_mgr.h"
#include "cfg.h"

int64_t DBMgr::CreateUserAccount(const SignupRequest& req) {
  return CreateUserAccount("phone", req.phone(), req.name(), req.passwd());
}

bool DBMgr::GetUidByPhone(const std::string& phone, int64_t *uid_ptr) {
  char sql[255];
  snprintf(sql, sizeof sql, "select uid from user where phone='%s'", phone.c_str());
  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }
  int64_t uid = -1;
  /* next, print out the rows */
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    uid = atol(PQgetvalue(res, i, 0));
    if (uid >= 0) break;
  }

  PQclear(res);
  *uid_ptr = uid;
  return true;
}

int64_t DBMgr::CreateUserAccount(const std::string& id_type, const std::string& openid, const std::string& name, const std::string& passwd) {
  if (pg_conn_ == nullptr) {
    return -1;
  }
  std::unique_lock<std::mutex> _lock(uid_mutex_);
  char sql[1024];
  snprintf(sql, sizeof sql, "select max(uid) from user");
  auto res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SQL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }
  int64_t uid = -1;
  for (int i = 0; i < PQntuples(res); i++) {
    char *val = PQgetvalue(res, i, 0);
    if (val != nullptr && val[0] != 0) {
      uid = atol(val);
    }
  }

  PQclear(res);
  if (uid < 0) return -1;
  ++uid;
  snprintf(sql, sizeof sql,
    "insert into user(uid,name,%s,passwd) values(%lu,'%s','%s','%s')",
    id_type.c_str(), uid,
    name.empty()?std::to_string(uid).c_str():name.c_str(),
    openid.c_str(), passwd.c_str());

  res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "sql(%s) failed: %s\n", sql, PQerrorMessage(pg_conn_));
    PQclear(res);
    return -1;
  }

  PQclear(res);
  return uid;
}

std::string DBMgr::CreateLoginSession(int64_t uid, const std::string& device_key) {
  if (uid < 0 || device_key.empty()) return "";
  std::string session_str = std::to_string(uid);
  session_str += "-" + device_key + '-';
  session_str += std::to_string(time(0));

  if (pg_conn_ == nullptr) {
    return "";
  }

  char sql[1024];
  snprintf(sql, sizeof sql,
    "insert into app_session(uid,session,device_key) values(%lu,'%s','%s')",
    uid, session_str.c_str(), device_key.c_str());

  auto res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "sql(%s) failed: %s\n", sql, PQerrorMessage(pg_conn_));
    PQclear(res);
    return "";
  }

  PQclear(res);
  VLOG(60) << " session = [" << session_str << "] len " << session_str.size();
  return session_str;
}

bool DBMgr::SaveSessionPhone(const std::string& session, const std::string& phone) {
  if (pg_conn_ == nullptr) {
    return false;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "update verify_code set phone='%s' where session = '%s'", phone.c_str(), session.c_str());
  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "SQL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }

  PQclear(res);
  return true;
}

bool DBMgr::VerifyPicVericode(const std::string& session, const std::string& code) {
  if (pg_conn_ == nullptr) {
    return false;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "select count(*) from verify_code where session = '%s' and pic_code = '%s' "
	"and timestamp + interval '6000 minutes' >= now()", session.c_str(), code.c_str());

  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SQL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }
  int count = 0;
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    count = atol(PQgetvalue(res, i, 0));
    break;
  }

  PQclear(res);
  return count > 0;
}

bool DBMgr::VerifySmsVericode(const std::string& session,
    const std::string& phone, const std::string& code) {
  if (pg_conn_ == nullptr) {
    return false;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "select count(*) from verify_code where session='%s' "
    "and phone='%s' and sms_code='%s' and timestamp + interval '6000 minutes' >= now()",
    session.c_str(), phone.c_str(), code.c_str());

  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SQL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }
  int count = 0;
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    count = atol(PQgetvalue(res, i, 0));
    break;
  }

  PQclear(res);
  return count > 0;
}

static std::string GetVericode(PGconn *conn, const std::string& session, const std::string& name) {
  std::string code;
  if (conn == nullptr) {
    return code;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "select %s_code from verify_code where session = '%s'", name.c_str(), session.c_str());
  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "SQL failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return code;
  }
  /* next, print out the rows */
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    code = PQgetvalue(res, i, 0);
    break;
  }

  PQclear(res);
  return code;
}

std::string DBMgr::GetPicVericode(const std::string& session) {
  return GetVericode(pg_conn_, session, "pic");
}

std::string DBMgr::GetSmsVericode(const std::string& session) {
  return GetVericode(pg_conn_, session, "sms");
}

static bool SaveVericode(PGconn *conn, const std::string& session, const std::string& code, const std::string& name) {
  if (conn == nullptr) {
    return false;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "select count(0) from verify_code where session = '%s'", session.c_str());
  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
  }

  bool session_found = false;
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    auto count = atol(PQgetvalue(res, i, 0));
    if (count > 0) {
      session_found = true;
    }
  }
  PQclear(res);

  if (session_found) {
    snprintf(sql, sizeof sql,
      "update verify_code set %s_code='%s',timestamp=now() where session='%s'",
      name.c_str(), code.c_str(), session.c_str());
  } else {
    snprintf(sql, sizeof sql,
      "insert into verify_code(session,%s_code) values('%s','%s')",
      name.c_str(), session.c_str(), code.c_str());
  }

  res = PQexec(conn, sql);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "sql(%s) failed: %s\n", sql, PQerrorMessage(conn));
    PQclear(res);
    return false;
  }

  PQclear(res);
  return true;
}

bool DBMgr::SavePicVericode(const std::string& session, const std::string& code) {
  return SaveVericode(pg_conn_, session, code, "pic");
}

bool DBMgr::SaveSmsVericode(const std::string& session, const std::string& code) {
  return SaveVericode(pg_conn_, session, code, "sms");
}

bool DBMgr::GetUserBasicInfo(int64_t uid, const char *phone, const char *openid, UserBasicInfo *out) {
  if (pg_conn_ == nullptr) {
    return false;
  }
  if (openid != nullptr) {
    uid = GetUidByOpenId(openid);
  }
  std::string where;
  if (uid != 0) {
    where = "uid=" + std::to_string(uid);
  } else if (phone != nullptr) {
    char buf[256];
    snprintf(buf, sizeof buf, "phone='%s'", phone);
    where = buf;
  } else {
    return false;
  }
  char sql[1024];
  snprintf(sql, sizeof sql, "select uid,name,phone,passwd,gender,image from user where %s", where.c_str());
  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return false;
  }
  if (PQntuples(res) <= 0) {
    PQclear(res);
    return true;
  }
  /* next, print out the rows */
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    auto uid = atol(PQgetvalue(res, i, 0));
    out->set_uid(uid);
    const char *name = PQgetvalue(res, i, 1);
    out->set_name(name);
    const char *phone_num = PQgetvalue(res, i, 2);
    out->set_phone(phone_num);
    const char *passwd = PQgetvalue(res, i, 3);
    out->set_passwd(passwd);
    auto gender = atoi(PQgetvalue(res, i, 4));
    out->set_gender(static_cast<UserBasicInfo_Gender>(gender));
    const char *image = PQgetvalue(res, i, 5);
    out->set_image(image);
    break;
  }

  PQclear(res);
  return true;
}

int64_t DBMgr::GetUidByOpenId(const char *openid) {
  char sql[255];
  snprintf(sql, sizeof sql, "select uid from user_openid where openid='%s'", openid);
  PGresult *res = PQexec(pg_conn_, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(pg_conn_));
    PQclear(res);
    return 0;
  }
  int64_t uid = 0;
  /* next, print out the rows */
  for (int i = 0; i < PQntuples(res); i++) {
      // for (j = 0; j < nFields; j++)
    uid = atol(PQgetvalue(res, i, 0));
  }

  PQclear(res);
  return uid;
}

DBMgr::DBMgr(const char *conninfo) {
  // conn_str: "postgresql://y@localhost/postgres"
  pg_conn_ = PQconnectdb(conninfo);    
  /* Check to see that the backend connection was successfully made */
  if (PQstatus(pg_conn_) != CONNECTION_OK) {
    fprintf(stderr, "Connection to database failed: %s",
      PQerrorMessage(pg_conn_));
    PQfinish(pg_conn_);
    pg_conn_ = nullptr;
  } else {
    fprintf(stderr, "PostgreSQL server version %i, protocol %i\n", PQserverVersion(pg_conn_), PQprotocolVersion(pg_conn_));
  }
}

DBMgr::~DBMgr() {
  PQfinish(pg_conn_);
}
