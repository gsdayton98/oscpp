// Copyright 2016. Glen S. Dayton.  All rights reserved.
#include <cstdio>
#include <cstring>
#include <dlfcn.h>

namespace {
}
class MockStrError {
public:
  MockStrError(void);

  int strerror_r(int errnum, char *strerrbuf, size_t buflen);

private:
  static extern "C" int (*realStrError_r)(int, char*, size_t);
};

extern "C" int (*MockStrError::realStrError_r)(int, char*, size_t) = nullptr;

MockStrError::MockStrError(void) {
  if (realStrError_r == nullptr) {
    realStrError_r = dlsym(RTLD_NEXT, "strerror_r");
  }
}


extern "C" int strerror_r(int errnum, char *strerrbuf, size_t buflen) {

}


