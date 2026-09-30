// -*- mode: c++ -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cerrno>
#include <string>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include "oscpp_exceptionless/file.hpp"
#include "temp_directory.hpp"

BOOST_AUTO_TEST_SUITE(ExceptionlessFile)

constexpr unsigned int NUMBER_POINTS = 256u;

// Each test case gets its own temporary directory holding a data file and an empty file; both are removed afterward.
struct DataFile {
    TempDirectory directory;
    std::string dataFileName {directory.path("data.dat")};
    std::string emptyFileName {directory.path("empty.dat")};

    DataFile() {
        std::ofstream out(dataFileName, std::ios::out | std::ios::binary);
        for (unsigned int i = 0; i < NUMBER_POINTS; ++i) {
            out.write(reinterpret_cast<char *>(&i), sizeof(i));
        }
        std::ofstream empty(emptyFileName, std::ios::out | std::ios::trunc);
    }
};

BOOST_FIXTURE_TEST_CASE(testFstat, DataFile) {
    auto file = oscpp_exceptionless::File::create(dataFileName.c_str());
    BOOST_REQUIRE(file.has_value());
    const auto stats = file->fstat();
    BOOST_REQUIRE(stats.has_value());
    BOOST_CHECK_EQUAL(static_cast<unsigned long>(stats->st_size), NUMBER_POINTS * sizeof(unsigned int));
}

BOOST_FIXTURE_TEST_CASE(testMap, DataFile) {
    auto file = oscpp_exceptionless::File::create(dataFileName.c_str());
    BOOST_REQUIRE(file.has_value());
    const auto mapped = file->map();
    BOOST_REQUIRE(mapped.has_value());
    BOOST_CHECK_EQUAL(mapped->second, NUMBER_POINTS * sizeof(unsigned int));
    const auto *values = static_cast<const unsigned int *>(mapped->first);
    BOOST_CHECK_EQUAL(values[1], 1u);
    BOOST_CHECK_EQUAL(values[NUMBER_POINTS - 1], NUMBER_POINTS - 1u);
    // Mapping again replaces the first mapping.
    BOOST_CHECK(file->map().has_value());
}

BOOST_FIXTURE_TEST_CASE(testMapEmptyFile, DataFile) {
    auto file = oscpp_exceptionless::File::create(emptyFileName.c_str());
    BOOST_REQUIRE(file.has_value());
    const auto mapped = file->map();
    BOOST_REQUIRE(mapped.has_value());
    BOOST_CHECK(mapped->first == nullptr);
    BOOST_CHECK_EQUAL(mapped->second, 0u);
}

BOOST_AUTO_TEST_CASE(testOpenFails) {
    const auto file = oscpp_exceptionless::File::create("/nonexistent/exceptionless/file");
    BOOST_REQUIRE(!file.has_value());
    BOOST_CHECK_EQUAL(file.error().value(), ENOENT);
}

BOOST_FIXTURE_TEST_CASE(testMove, DataFile) {
    auto file = oscpp_exceptionless::File::create(dataFileName.c_str());
    BOOST_REQUIRE(file.has_value());
    BOOST_REQUIRE(file->map().has_value());
    oscpp_exceptionless::File moved {std::move(*file)};
    BOOST_CHECK(moved.fstat().has_value());
}

BOOST_AUTO_TEST_SUITE_END()
