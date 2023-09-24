// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.

#include <CppUnitXLite/CppUnitXLite.hpp>
#include <sysexception.hpp>
using std::string;



TEST(oscpp, testSystemException) {
  const char *expected[] = {
    "Undefined error: 0",
    "Operation not permitted"
  };


  static const int N_TEST_CASES = sizeof(expected) / sizeof(expected[0]);

  for (int err = 0; err < N_TEST_CASES; ++err) {
    oscpp::SysException ex(err);
    CHECK_EQUAL(string(expected[err]), string(ex.what()));
  }
}
