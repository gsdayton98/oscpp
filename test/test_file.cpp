// -*- mode: c++ -*-
////
//!  Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//!  Test File
#define BOOST_BOOST_AUTO_TEST_MODULE Test File

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "file.hpp"

constexpr unsigned int NUMBER_POINTS = 1024u;
constexpr auto testFileName = "testFile.dat";

struct TextFileFixture {

    TextFileFixture() {
        // Write a little file with sample data.

            std::ofstream testFile(testFileName, std::ios::out | std::ios::binary);
        if (!testFile.is_open()) {
            throw std::runtime_error("Failed to open test file for writing");
        }
        for (unsigned int i = 0; i < NUMBER_POINTS; ++i) {
            testFile.write(reinterpret_cast<char *>(&i), sizeof(i));
        }
    }

    ~TextFileFixture() = default;
};

BOOST_AUTO_TEST_SUITE(File)


BOOST_TEST_GLOBAL_FIXTURE(TextFileFixture);


BOOST_AUTO_TEST_CASE(Test_fstat)
{
    struct stat buffer {};

    oscpp::File file(testFileName);
    auto stat = file.fstat(buffer);
    BOOST_TEST_REQUIRE(stat.st_size == NUMBER_POINTS * sizeof(unsigned int));
}


BOOST_AUTO_TEST_CASE(Test_map)
{
    oscpp::File file(testFileName);
    auto [mappedFile, mappedLen] = file.map();
    BOOST_TEST_REQUIRE(mappedLen == NUMBER_POINTS * sizeof(unsigned int));
    BOOST_TEST_REQUIRE(mappedFile != nullptr);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[0] == 0u);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[1] == 1u);
    BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[NUMBER_POINTS - 1] == NUMBER_POINTS - 1u);
}

BOOST_AUTO_TEST_SUITE_END()