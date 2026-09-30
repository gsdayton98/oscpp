// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <boost/test/unit_test.hpp>
#include <type_traits>
#include <utility>
#include "oscpp_exceptionless/random_device.hpp"

// Nothing in oscpp_exceptionless may throw.
using ERD = oscpp_exceptionless::RandomDevice;
static_assert(noexcept(ERD::create()));
static_assert(noexcept(ERD::create("/dev/urandom")));
static_assert(noexcept(std::declval<ERD &>().next()));
static_assert(noexcept(std::declval<const ERD &>().entropy()));
static_assert(std::is_nothrow_move_constructible_v<ERD>);

BOOST_AUTO_TEST_SUITE(ExceptionlessRandomDevice)

BOOST_AUTO_TEST_CASE(testDefaultDevice) {
  auto device = oscpp_exceptionless::RandomDevice::create();
  BOOST_REQUIRE(device.has_value());
  for (int i = 0; i < 10; ++i) {
    const auto value = device->next();
    BOOST_REQUIRE(value.has_value());
    BOOST_CHECK(*value >= oscpp_exceptionless::RandomDevice::min());
  }
}

BOOST_AUTO_TEST_CASE(testTokenDevice) {
  auto device = oscpp_exceptionless::RandomDevice::create("/dev/urandom");
  BOOST_REQUIRE(device.has_value());
  BOOST_CHECK(device->next().has_value());
}

BOOST_AUTO_TEST_SUITE_END()
