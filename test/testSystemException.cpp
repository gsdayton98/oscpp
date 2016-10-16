// Copyright 2016.  Glen S. Dayton.  All rights reserved.
#include <CppUnitXLite/CppUnitXLite.hpp>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sysexception.hpp>

using std::string;



TEST(oscpp, testSystemException) {
  const char *expected[] = {
    "Undefined error: 0",
    "Operation not permitted"
  };
  static const int NTESTCASES = sizeof(expected) / sizeof(expected[0]);

  for (int err = 0; err < NTESTCASES; ++err) {
    oscpp::SysException ex(err);
    CHECK_EQUAL(string(expected[err]), string(ex.what()));
  }
}
