// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cstring>
#include <boost/test/unit_test.hpp>
#include "oscpp_exceptionless/dynamiclibrary.hpp"

BOOST_AUTO_TEST_SUITE(ExceptionlessDynamicLibrary)

BOOST_AUTO_TEST_CASE(testCurrentImage) {
    auto library = oscpp_exceptionless::DynamicLibrary::create();
    BOOST_REQUIRE(library.has_value());
    const auto symbol = library->symbol("strlen");
    BOOST_REQUIRE(symbol.has_value());
    BOOST_CHECK_EQUAL(reinterpret_cast<void *>(strlen), *symbol);
}

BOOST_AUTO_TEST_CASE(testMissingSymbol) {
    auto library = oscpp_exceptionless::DynamicLibrary::create();
    BOOST_REQUIRE(library.has_value());
    const auto symbol = library->symbol("oscpp_no_such_symbol");
    BOOST_REQUIRE(!symbol.has_value());
    BOOST_CHECK(!symbol.error().empty());
}

BOOST_AUTO_TEST_CASE(testMissingLibrary) {
    const auto library = oscpp_exceptionless::DynamicLibrary::create("/nonexistent/libnothing.dylib");
    BOOST_REQUIRE(!library.has_value());
    BOOST_CHECK(!library.error().empty());
}

BOOST_AUTO_TEST_SUITE_END()
