// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 04/25/2026.
//
#define BOOST_BOOST_AUTO_TEST_MODULE Test RandomDevice
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include "oscpp/random_device.hpp"

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

// The device must work as a UniformRandomBitGenerator with the standard library.
BOOST_AUTO_TEST_CASE(test_random_device_as_generator)
{
    oscpp::RandomDevice rd;

    std::uniform_int_distribution<int> dist{1, 6};
    for (int i = 0; i < 100; ++i) {
        const int v = dist(rd);
        BOOST_REQUIRE(v >= 1 && v <= 6);
    }

    std::vector<int> values(32);
    std::iota(values.begin(), values.end(), 0);
    std::shuffle(values.begin(), values.end(), rd);
    std::sort(values.begin(), values.end());
    for (int i = 0; i < 32; ++i) {
        BOOST_REQUIRE_EQUAL(values[static_cast<std::size_t>(i)], i);
    }
}

BOOST_AUTO_TEST_CASE(test_random_device_token)
{
    oscpp::RandomDevice rd{"/dev/urandom"};
    BOOST_REQUIRE_NO_THROW(rd());
}

BOOST_AUTO_TEST_SUITE_END()
