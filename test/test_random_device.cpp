// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 04/25/2026.
//
#define BOOST_BOOST_AUTO_TEST_MODULE Test RandomDevice
#include <boost/test/unit_test.hpp>

#include "random_device.hpp"

BOOST_AUTO_TEST_SUITE(RandomDevice)

// This test is mostly to ensure it compiles and runs without errors.
BOOST_AUTO_TEST_CASE(test_random_device)
{
    oscpp::RandomDevice rd;

    const auto f = rd.entropy();
    BOOST_REQUIRE(f > 0);
    BOOST_TEST_MESSAGE("entropy " << f);
    BOOST_TEST_MESSAGE("min " << oscpp::RandomDevice::min());
    BOOST_TEST_MESSAGE("max " << oscpp::RandomDevice::max());
    BOOST_TEST_MESSAGE("random value: " << rd());
}

BOOST_AUTO_TEST_SUITE_END()