#ifndef SEA_PQ_POOL_H_
#define SEA_PQ_POOL_H_

#include <libpq-fe.h>

class PQpool {
 public:
  PQpool(const char *connStr, int size) {
  }
  PGconn *Pop() {
    return nullptr;
  }
  void Push(PGconn *conn) {
  }
};

#endif // SEA_PQ_POOL_H_
