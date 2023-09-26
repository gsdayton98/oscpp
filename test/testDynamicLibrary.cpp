// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_BOOST_AUTO_TEST_MODULE Test systemDynamicLibrary
#include <boost/test/unit_test.hpp>
#include <string>
#include "sysexception.hpp"
#include "dynamiclibrary.hpp"
using std::string;


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
#pragma clang diagnostic pop