// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cstring>
#include <utility>
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

BOOST_AUTO_TEST_CASE(testFlags) {
    auto lazy = oscpp_exceptionless::DynamicLibrary::create(RTLD_LAZY | RTLD_LOCAL);
    BOOST_REQUIRE(lazy.has_value());
    BOOST_CHECK(lazy->symbol("strlen").has_value());
}

BOOST_AUTO_TEST_CASE(testMoveAssignment) {
    auto first = oscpp_exceptionless::DynamicLibrary::create();
    auto second = oscpp_exceptionless::DynamicLibrary::create(RTLD_LAZY | RTLD_LOCAL);
    BOOST_REQUIRE(first.has_value() && second.has_value());
    *second = std::move(*first);
    BOOST_CHECK(second->symbol("strlen").has_value());

    // Assigning to itself must not close the library.
    auto &alias = *second;
    *second = std::move(alias);
    BOOST_CHECK(second->symbol("strlen").has_value());
}

BOOST_AUTO_TEST_SUITE_END()
