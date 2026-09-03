// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
////
//!  Test Stopwatch
#define BOOST_BOOST_AUTO_TEST_MODULE Test StopWatch

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <boost/test/unit_test.hpp>
#include "stopwatch.hpp"
BOOST_AUTO_TEST_SUITE(StopWatch)
BOOST_AUTO_TEST_CASE(test_stopwatch)
{
    constexpr auto SECOND = 1000000UL;
    oscpp::StopWatch stopwatch;
    usleep(SECOND);
    auto reading = stopwatch.read();
    BOOST_REQUIRE_LT(1.0, reading);

    usleep(SECOND);
    reading = stopwatch.read();
    BOOST_REQUIRE_LT(2.0, reading);

    stopwatch.reset();
    usleep(SECOND);
    reading = stopwatch.read();
    BOOST_REQUIRE_LT(1.0, reading);
}
BOOST_AUTO_TEST_SUITE_END()