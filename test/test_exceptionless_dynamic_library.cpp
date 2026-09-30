// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>
#include <boost/test/unit_test.hpp>
#include "oscpp_exceptionless/dynamiclibrary.hpp"

// Nothing in oscpp_exceptionless may throw.
using DL = oscpp_exceptionless::DynamicLibrary;
static_assert(noexcept(DL::create()));
static_assert(noexcept(DL::create("path")));
static_assert(noexcept(std::declval<const DL &>().symbol("name")));
static_assert(std::is_nothrow_move_constructible_v<DL> && std::is_nothrow_move_assignable_v<DL>);
static_assert(noexcept(oscpp_exceptionless::DynamicLibraryError::fromText("text")));
static_assert(noexcept(oscpp_exceptionless::DynamicLibraryError::fromDlerror()));
static_assert(std::is_nothrow_copy_constructible_v<oscpp_exceptionless::DynamicLibraryError>);

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
    BOOST_CHECK(std::strlen(symbol.error().what()) > 0);
}

BOOST_AUTO_TEST_CASE(testMissingLibrary) {
    const auto library = oscpp_exceptionless::DynamicLibrary::create("/nonexistent/libnothing.dylib");
    BOOST_REQUIRE(!library.has_value());
    BOOST_CHECK(std::strlen(library.error().what()) > 0);
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

// The error text is copied into a fixed buffer: short text is kept, long text is truncated, a null pointer is an empty message.
BOOST_AUTO_TEST_CASE(testErrorText) {
    using Error = oscpp_exceptionless::DynamicLibraryError;
    BOOST_CHECK_EQUAL(std::string{Error::fromText("short").what()}, "short");

    const std::string longText(1000, 'x');
    const auto truncated = Error::fromText(longText.c_str());
    BOOST_CHECK_EQUAL(std::strlen(truncated.what()), Error::Capacity - 1);
    BOOST_CHECK_EQUAL(truncated.message[Error::Capacity - 1], '\0');

    // dlerror() returns null when no error has occurred since its last call, which is an empty message.
    BOOST_CHECK_EQUAL(std::string{Error::fromText(nullptr).what()}, "");
}

BOOST_AUTO_TEST_SUITE_END()
