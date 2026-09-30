// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016.  Glen S. Dayton. Rights reserved according to terms of included license.

#include <boost/test/unit_test.hpp>
#include "oscpp/trim.hpp"

using std::string;
BOOST_AUTO_TEST_SUITE(Trim)

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
    // back() on an empty string is undefined, so only check the last character of non-empty results.
    if (!testCase.empty()) {
      BOOST_CHECK_EQUAL((int) expectedLastCharacter[testNumber], (int) testCase.back());
    }
  }
}

BOOST_AUTO_TEST_CASE(test_trim_empty) {
  string s;
  oscpp::trim(s);
  BOOST_CHECK(s.empty());
}

BOOST_AUTO_TEST_CASE(test_trim_all_whitespace) {
  for (const string &original : {string{" "}, string{"   "}, string{"\t\n\r \v\f"}, string{"\0", 1}, string{" \t\0 \n", 5}}) {
    string s = original;
    oscpp::trim(s);
    BOOST_CHECK_MESSAGE(s.empty(), "not empty after trimming a string of length " << original.size());
  }
}

// Only the trailing end is trimmed.
BOOST_AUTO_TEST_CASE(test_trim_keeps_leading_whitespace) {
  string s = "  inside  spaces  ";
  oscpp::trim(s);
  BOOST_CHECK_EQUAL(s, "  inside  spaces");
}

// Bytes with the high bit set (for example UTF-8 text) are negative as plain char; they must not reach isspace() as such.
BOOST_AUTO_TEST_CASE(test_trim_high_bit_characters) {
  string s = "caf\xc3\xa9 \t";
  oscpp::trim(s);
  BOOST_CHECK_EQUAL(s, "caf\xc3\xa9");

  string onlyHighBit = "\xff\xfe";
  oscpp::trim(onlyHighBit);
  BOOST_CHECK_EQUAL(onlyHighBit.size(), 2u);
}

BOOST_AUTO_TEST_SUITE_END()