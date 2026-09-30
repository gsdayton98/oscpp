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
#include "oscpp/stopwatch.hpp"
BOOST_AUTO_TEST_SUITE(StopWatch)
BOOST_AUTO_TEST_CASE(test_stopwatch)
{
    constexpr auto INTERVAL = 50000UL; // microseconds
    constexpr auto INTERVAL_SECONDS = 0.05;
    oscpp::StopWatch stopwatch;
    usleep(INTERVAL);
    auto reading = stopwatch.read();
    BOOST_REQUIRE_LE(INTERVAL_SECONDS, reading);

    usleep(INTERVAL);
    const auto secondReading = stopwatch.read();
    BOOST_REQUIRE_LE(2 * INTERVAL_SECONDS, secondReading);
    BOOST_REQUIRE_LT(reading, secondReading);
}

BOOST_AUTO_TEST_CASE(test_stopwatch_reset)
{
    constexpr auto INTERVAL = 50000UL; // microseconds
    constexpr auto INTERVAL_SECONDS = 0.05;
    oscpp::StopWatch stopwatch;
    usleep(2 * INTERVAL);
    const auto beforeReset = stopwatch.read();
    BOOST_REQUIRE_LE(2 * INTERVAL_SECONDS, beforeReset);

    stopwatch.reset();
    // Immediately after a reset the reading restarts near zero, well below the time already elapsed.
    BOOST_CHECK_LT(stopwatch.read(), beforeReset);
    usleep(INTERVAL);
    const auto afterReset = stopwatch.read();
    BOOST_CHECK_LE(INTERVAL_SECONDS, afterReset);
    BOOST_CHECK_LT(afterReset, beforeReset + INTERVAL_SECONDS);
}

BOOST_AUTO_TEST_SUITE_END()