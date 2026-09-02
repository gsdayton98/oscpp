// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016.  Glen S. Dayton. Rights reserved according to terms of included license.

#define BOOST_BOOST_AUTO_TEST_MODULE Test trim
#include <boost/test/unit_test.hpp>
#include "trim.hpp"

using std::string;

BOOST_AUTO_TEST_CASE(test_trim) {
  string sample[] = {
    "The rain in Spain falls mainly on the plain     \t\0\0\0 ", // NOLINT(*-string-literal-with-embedded-nul)
    "The quick brown fox jumps over the lazy dog.",
    ""
  };
  size_t expected[] = {
    43,
    44,
    0
  };
  char expectedLastCharacter[] = {
    'n',
    '.',
    0
  };

  const size_t NumberSamples = sizeof(sample) / sizeof(sample[0]);
  for (size_t testNumber = 0; testNumber < NumberSamples; ++testNumber) {
    string testCase = sample[testNumber];
    oscpp::trim(testCase);
    BOOST_CHECK_EQUAL(expected[testNumber], testCase.size());
    BOOST_CHECK_EQUAL((int) expectedLastCharacter[testNumber], (int) testCase.back());
  }
}