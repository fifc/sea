
#ifndef AKAGO_CONFIG_H
#define AKAGO_CONFIG_H

#include <gflags/gflags.h>
#include <glog/logging.h>

DECLARE_string(addr);
DECLARE_string(port);
DECLARE_string(key);
DECLARE_string(cert);
DECLARE_string(db_cfg);
DECLARE_int32(threads);

namespace cfg {
	bool init(int argc, char *argv[]);
	void cleanup();
}

#endif // AKAGO_CONFIG_H
