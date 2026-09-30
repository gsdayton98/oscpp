// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.

#define BOOST_BOOST_AUTO_TEST_MODULE Test systemException
#include <boost/test/unit_test.hpp>
#include "oscpp/sysexception.hpp"
using std::string;
BOOST_AUTO_TEST_SUITE(SystemException)

BOOST_AUTO_TEST_CASE(test_system_exception) {
  const char *expected[] = {
    "Undefined error: 0",
    "Operation not permitted"
  };


  constexpr std::size_t N_TEST_CASES = sizeof(expected) / sizeof(expected[0]);

  for (int err = 0; err < static_cast<int>(N_TEST_CASES); ++err) {
    oscpp::SysException ex(err);
    BOOST_CHECK_EQUAL(string(expected[err]), string(ex.what()));
  }
}

BOOST_AUTO_TEST_CASE(test_system_exception_code) {
  const oscpp::SysException fromErrno(ENOENT);
  BOOST_CHECK_EQUAL(fromErrno.code().value(), ENOENT);
  BOOST_CHECK(fromErrno.code().category() == std::generic_category());
  BOOST_CHECK(fromErrno.code() == std::errc::no_such_file_or_directory);

  const oscpp::SysException fromCode(std::make_error_code(std::errc::permission_denied));
  BOOST_CHECK(fromCode.code() == std::errc::permission_denied);
}

// Callers can catch it as a std::system_error, or still as a std::runtime_error.
BOOST_AUTO_TEST_CASE(test_system_exception_base) {
  BOOST_CHECK_THROW(throw oscpp::SysException(EACCES), std::system_error);
  BOOST_CHECK_THROW(throw oscpp::SysException(EACCES), std::runtime_error);
}
BOOST_AUTO_TEST_SUITE_END()