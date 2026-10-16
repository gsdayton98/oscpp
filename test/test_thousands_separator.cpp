// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cstdint>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include "oscpp/thousands_separator.hpp"

namespace {
/// Write a value to a stream that has been given the locale `locale`.
template <typename Number> auto written(const std::locale &locale, Number value) -> std::string {
  std::ostringstream out;
  out.imbue(locale);
  out << value;
  return out.str();
}

auto commas() -> std::locale {
  return oscpp::withThousandsSeparator(std::locale::classic());
}
} // namespace

BOOST_AUTO_TEST_SUITE(ThousandsSeparator)

BOOST_AUTO_TEST_CASE(test_groups_of_three) {
  const auto locale = commas();
  BOOST_CHECK_EQUAL(written(locale, 0), "0");
  BOOST_CHECK_EQUAL(written(locale, 5), "5");
  BOOST_CHECK_EQUAL(written(locale, 999), "999");
  BOOST_CHECK_EQUAL(written(locale, 1000), "1,000");
  BOOST_CHECK_EQUAL(written(locale, 12345), "12,345");
  BOOST_CHECK_EQUAL(written(locale, 123456), "123,456");
  BOOST_CHECK_EQUAL(written(locale, 1234567), "1,234,567");
  BOOST_CHECK_EQUAL(written(locale, 1000000), "1,000,000");
}

BOOST_AUTO_TEST_CASE(test_negative_numbers) {
  const auto locale = commas();
  BOOST_CHECK_EQUAL(written(locale, -1), "-1");
  BOOST_CHECK_EQUAL(written(locale, -999), "-999");
  BOOST_CHECK_EQUAL(written(locale, -1000), "-1,000");
  BOOST_CHECK_EQUAL(written(locale, -1234567), "-1,234,567");
}

BOOST_AUTO_TEST_CASE(test_widest_integers) {
  const auto locale = commas();
  BOOST_CHECK_EQUAL(written(locale, std::numeric_limits<std::uint64_t>::max()), "18,446,744,073,709,551,615");
  BOOST_CHECK_EQUAL(written(locale, std::numeric_limits<std::int64_t>::min()), "-9,223,372,036,854,775,808");
}

// Only the digits left of the decimal point are grouped, and the decimal point itself is the base locale's.
BOOST_AUTO_TEST_CASE(test_floating_point) {
  std::ostringstream out;
  out.imbue(commas());
  out << std::fixed << std::setprecision(2) << 1234567.891 << " " << 999.5 << " " << 0.125;
  BOOST_CHECK_EQUAL(out.str(), "1,234,567.89 999.50 0.12");
}

BOOST_AUTO_TEST_CASE(test_other_separator_and_group_size) {
  BOOST_CHECK_EQUAL(written(oscpp::withThousandsSeparator(std::locale::classic(), '.'), 1234567), "1.234.567");
  BOOST_CHECK_EQUAL(written(oscpp::withThousandsSeparator(std::locale::classic(), ' ', 4), 12345678), "1234 5678");
  BOOST_CHECK_EQUAL(written(oscpp::withThousandsSeparator(std::locale::classic(), ' ', 4), 1234567), "123 4567");
}

BOOST_AUTO_TEST_CASE(test_group_size_zero_means_no_grouping) {
  BOOST_CHECK_EQUAL(written(oscpp::withThousandsSeparator(std::locale::classic(), ',', 0), 1234567), "1234567");
}

// The facet belongs to the locale that was made, so other streams, and the locale it was built from, are unchanged.
BOOST_AUTO_TEST_CASE(test_other_streams_are_unaffected) {
  const auto locale = commas();
  BOOST_CHECK_EQUAL(written(locale, 1234567), "1,234,567");
  BOOST_CHECK_EQUAL(written(std::locale::classic(), 1234567), "1234567");

  std::ostringstream plain;
  plain << 1234567;
  BOOST_CHECK_EQUAL(plain.str(), "1234567");
}

// The usage in the header's documentation: imbue the stream's own locale, then write.
BOOST_AUTO_TEST_CASE(test_imbue_a_streams_own_locale) {
  std::ostringstream out;
  out.imbue(oscpp::withThousandsSeparator(out.getloc()));
  out << 1234567 << " " << 42;
  BOOST_CHECK_EQUAL(out.str(), "1,234,567 42");
}

BOOST_AUTO_TEST_CASE(test_the_facet_itself) {
  const oscpp::ThousandsSeparator facet;
  BOOST_CHECK_EQUAL(facet.thousands_sep(), ',');
  BOOST_CHECK_EQUAL(facet.grouping(), std::string("\3"));
  BOOST_CHECK_EQUAL(facet.decimal_point(), '.');
  BOOST_CHECK(oscpp::ThousandsSeparator(' ', 0).grouping().empty());
}

BOOST_AUTO_TEST_SUITE_END()
