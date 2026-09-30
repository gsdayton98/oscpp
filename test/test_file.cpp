// -*- mode: c++ -*-
////
//!  Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//!  Test File
#define BOOST_BOOST_AUTO_TEST_MODULE Test File

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "oscpp/file.hpp"
#include "temp_directory.hpp"

constexpr unsigned int NUMBER_POINTS = 1024u;

// Each test case gets its own temporary directory holding a data file and an empty file; both are removed afterward.
struct TextFileFixture {
    TempDirectory directory;
    std::string testFileName {directory.path("testFile.dat")};
    std::string emptyFileName {directory.path("emptyFile.dat")};

    TextFileFixture() {
        // Write a little file with sample data.
        std::ofstream testFile(testFileName, std::ios::out | std::ios::binary);
        if (!testFile.is_open()) {
            throw std::runtime_error("Failed to open test file for writing");
        }
        for (unsigned int i = 0; i < NUMBER_POINTS; ++i) {
            testFile.write(reinterpret_cast<char *>(&i), sizeof(i));
        }
        std::ofstream empty(emptyFileName, std::ios::out | std::ios::trunc);
    }
};

BOOST_AUTO_TEST_SUITE(File)


BOOST_FIXTURE_TEST_CASE(Test_fstat, TextFileFixture)
{
    struct stat buffer {};

    oscpp::File file(testFileName.c_str());
    auto stat = file.fstat(buffer);
    BOOST_TEST_REQUIRE(stat.st_size == NUMBER_POINTS * sizeof(unsigned int));
}


BOOST_FIXTURE_TEST_CASE(Test_map, TextFileFixture)
{
    oscpp::File file(testFileName.c_str());
    auto [mappedFile, mappedLen] = file.map();
    BOOST_TEST_REQUIRE(mappedLen == NUMBER_POINTS * sizeof(unsigned int));
    BOOST_TEST_REQUIRE(mappedFile != nullptr);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[0] == 0u);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[1] == 1u);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[NUMBER_POINTS - 1] == NUMBER_POINTS - 1u);
}

BOOST_FIXTURE_TEST_CASE(Test_map_empty_file, TextFileFixture)
{
    oscpp::File file(emptyFileName.c_str());
    auto [mappedFile, mappedLen] = file.map();
    BOOST_TEST_REQUIRE(mappedFile == nullptr);
    BOOST_TEST_REQUIRE(mappedLen == 0u);
}

BOOST_AUTO_TEST_SUITE_END()