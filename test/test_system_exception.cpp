// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_BOOST_AUTO_TEST_MODULE Test systemException
#include <boost/test/unit_test.hpp>
#include "sysexception.hpp"
using std::string;


BOOST_AUTO_TEST_CASE(test_system_exception) {
  const char *expected[] = {
    "Undefined error: 0",
    "Operation not permitted"
  };


  constexpr std::size_t N_TEST_CASES = sizeof(expected) / sizeof(expected[0]);

  for (int err = 0; err < N_TEST_CASES; ++err) {
    oscpp::SysException ex(err);
    BOOST_CHECK_EQUAL(string(expected[err]), string(ex.what()));
  }
}
