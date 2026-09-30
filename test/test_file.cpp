// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Test File

#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "oscpp/file.hpp"
#include "oscpp/sysexception.hpp"
#include "temp_directory.hpp"

constexpr unsigned int NUMBER_POINTS = 1024u;

// Each test case gets its own temporary directory holding a data file and an empty file; both are removed afterward.
struct TextFileFixture {
  TempDirectory directory;
  std::string testFileName{directory.path("testFile.dat")};
  std::string emptyFileName{directory.path("emptyFile.dat")};

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

BOOST_FIXTURE_TEST_CASE(Test_fstat, TextFileFixture) {
  struct stat buffer{};

  oscpp::File file(testFileName.c_str());
  auto stat = file.fstat(buffer);
  BOOST_TEST_REQUIRE(stat.st_size == NUMBER_POINTS * sizeof(unsigned int));
}

BOOST_FIXTURE_TEST_CASE(Test_map, TextFileFixture) {
  oscpp::File file(testFileName.c_str());
  auto [mappedFile, mappedLen] = file.map();
  BOOST_TEST_REQUIRE(mappedLen == NUMBER_POINTS * sizeof(unsigned int));
  BOOST_TEST_REQUIRE(mappedFile != nullptr);
  BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[0] == 0u);
  BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[1] == 1u);
  BOOST_TEST_REQUIRE(static_cast<unsigned int *>(mappedFile)[NUMBER_POINTS - 1] == NUMBER_POINTS - 1u);
}

BOOST_FIXTURE_TEST_CASE(Test_map_empty_file, TextFileFixture) {
  oscpp::File file(emptyFileName.c_str());
  auto [mappedFile, mappedLen] = file.map();
  BOOST_TEST_REQUIRE(mappedFile == nullptr);
  BOOST_TEST_REQUIRE(mappedLen == 0u);
}

BOOST_AUTO_TEST_CASE(Test_open_failure) {
  const TempDirectory directory;
  const auto missing = directory.path("does_not_exist.dat");
  try {
    oscpp::File file(missing.c_str());
    BOOST_FAIL("expected oscpp::SysException");
  } catch (const oscpp::SysException &ex) {
    BOOST_CHECK(ex.code() == std::errc::no_such_file_or_directory);
  }
}

BOOST_FIXTURE_TEST_CASE(Test_move_construction, TextFileFixture) {
  oscpp::File original(testFileName.c_str());
  oscpp::File moved(std::move(original));

  struct stat buffer{};
  BOOST_CHECK_EQUAL(static_cast<unsigned long>(moved.fstat(buffer).st_size), NUMBER_POINTS * sizeof(unsigned int));
  // The source gave up its descriptor.
  BOOST_CHECK_THROW(original.fstat(buffer), oscpp::SysException); // NOLINT(bugprone-use-after-move)
}

BOOST_FIXTURE_TEST_CASE(Test_remap, TextFileFixture) {
  oscpp::File file(testFileName.c_str());
  const auto first = file.map();
  const auto second = file.map();
  BOOST_TEST_REQUIRE(second.first != nullptr);
  BOOST_TEST_REQUIRE(first.second == second.second);
  BOOST_TEST_REQUIRE(static_cast<unsigned int *>(second.first)[NUMBER_POINTS - 1] == NUMBER_POINTS - 1u);
}

// Flags and mode are passed through to open(): create a new file writable-only with restrictive permissions.
BOOST_AUTO_TEST_CASE(Test_create_flags_and_mode) {
  const TempDirectory directory;
  const auto created = directory.path("created.dat");
  oscpp::File file(created.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);

  struct stat buffer{};
  file.fstat(buffer);
  BOOST_CHECK_EQUAL(buffer.st_size, 0);
  BOOST_CHECK_EQUAL(buffer.st_mode & 0777, 0600u);

  // O_EXCL makes a second create fail.
  BOOST_CHECK_THROW(oscpp::File(created.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600), oscpp::SysException);
}

BOOST_FIXTURE_TEST_CASE(Test_move_assignment, TextFileFixture) {
  oscpp::File first(testFileName.c_str());
  const int firstDescriptor = first.descriptor();
  {
    oscpp::File second(emptyFileName.c_str());
    const int secondDescriptor = second.descriptor();
    first = std::move(second);
    BOOST_CHECK(fcntl(firstDescriptor, F_GETFD) < 0);
    BOOST_CHECK_EQUAL(first.descriptor(), secondDescriptor);
    BOOST_CHECK(!second.valid()); // NOLINT(bugprone-use-after-move)
  }
  struct stat buffer{};
  BOOST_CHECK_EQUAL(first.fstat(buffer).st_size, 0);
}

BOOST_FIXTURE_TEST_CASE(Test_descriptor_and_valid, TextFileFixture) {
  oscpp::File file(testFileName.c_str());
  BOOST_CHECK(file);
  BOOST_CHECK(fcntl(file.descriptor(), F_GETFD) >= 0);
}

BOOST_FIXTURE_TEST_CASE(Test_unmap, TextFileFixture) {
  oscpp::File file(testFileName.c_str());
  (void)file.map();
  file.unmap();
  file.unmap(); // harmless when nothing is mapped
  const auto remapped = file.map();
  BOOST_TEST_REQUIRE(static_cast<unsigned int *>(remapped.first)[1] == 1u);
}

BOOST_FIXTURE_TEST_CASE(Test_release, TextFileFixture) {
  oscpp::File file(testFileName.c_str());
  const int descriptor = file.descriptor();
  BOOST_CHECK_EQUAL(file.release(), descriptor);
  BOOST_CHECK(!file);
  BOOST_CHECK(fcntl(descriptor, F_GETFD) >= 0); // not closed; the caller owns it
  close(descriptor);
  struct stat buffer{};
  BOOST_CHECK_THROW(file.fstat(buffer), oscpp::SysException);
}

BOOST_AUTO_TEST_SUITE_END()
