// Copyright 2016.  Glen S. Dayton.  All rights reserved.
#include <CppUnitXLite/CppUnitXLite.hpp>
#include <cstdlib>
#include <string>
#include <trim.hpp>

using std::string;

TEST(oscpp, testtrim) {
  string sample[] = {
    "The rain in Spain falls mainly on the plain     \t\0\0\0",
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
  const size_t NSAMPLE = sizeof(sample) / sizeof(sample[0]);

  std::cout << "testTrim" << std::endl;

  for (size_t testNumber = 0; testNumber < NSAMPLE; ++testNumber) {
    string testCase = sample[testNumber];
    oscpp::trim(testCase);
    CHECK_EQUAL(expected[testNumber], testCase.size());
    CHECK_EQUAL((int) expectedLastCharacter[testNumber], (int) testCase.back());
  }
}

