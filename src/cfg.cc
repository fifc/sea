
#include <iostream>
#include <curl/curl.h>

#include "cfg.h"

namespace gf = GFLAGS_NAMESPACE;

DEFINE_string(addr, "::", "listening address");
DEFINE_string(port, "8080", "listening port");
DEFINE_string(key, "", "ssl key file");
DEFINE_string(cert, "", "ssl cert file");
DEFINE_int32(threads, 2, "thread num");
DEFINE_string(db_cfg, "postgresql://y@localhost/sea", "postgresql url");

namespace cfg {
	bool init(int argc, char *argv[]) {
  		gf::ParseCommandLineFlags(&argc, &argv, false);
		fLS::FLAGS_log_dir = ".";
		google::InitGoogleLogging(argv[0]);
  		curl_global_init(CURL_GLOBAL_DEFAULT);
		LOG(ERROR) << "listening on: " << FLAGS_addr << " " << FLAGS_port;
		return true;
	}
	void cleanup() {
		curl_global_cleanup();
	}
}

