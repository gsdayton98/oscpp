// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#define BOOST_BOOST_AUTO_TEST_MODULE Test systemDynamicLibrary
#include <boost/test/unit_test.hpp>
#include <stdexcept>
#include <string>
#include "oscpp/dynamiclibrary.hpp"
using std::string;
BOOST_AUTO_TEST_SUITE(DynamicLibrary)


BOOST_AUTO_TEST_CASE(testSystemDynamicLibrary) {
    try {
        const auto target = "/usr/lib/libc++.1.dylib";
        oscpp::DynamicLibrary dynamicLibrary {target};
        auto queryResult = dynamicLibrary.symbol("strlen");
        BOOST_CHECK_EQUAL(reinterpret_cast<void *>(strlen), queryResult);

    }
    catch (const std::exception &ex) {
        BOOST_FAIL(ex.what());
    }
}

BOOST_AUTO_TEST_CASE(testMissingLibraryThrows) {
    BOOST_CHECK_THROW(oscpp::DynamicLibrary{"/nonexistent/libnothing.dylib"}, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(testMissingSymbolThrows) {
    const oscpp::DynamicLibrary currentImage;
    BOOST_CHECK_THROW((void) currentImage.symbol("oscpp_no_such_symbol"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()