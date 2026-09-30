// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Test helper: a unique scratch directory under the system temp directory, removed (with its contents) when the
// object is destroyed. Tests that write files use this so nothing lands in, or is left behind in, the working
// directory.

#ifndef OSCPP_TEST_TEMP_DIRECTORY_HPP
#define OSCPP_TEST_TEMP_DIRECTORY_HPP
#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>

/**
 * Some tests aren't real unit tests (in that they actually write to the file system).
 * TempDirectory creates a safe location for those test files on the system's temp file system.
 */
class TempDirectory {
  std::filesystem::path directory;

 public:
  TempDirectory() {
    auto pattern = (std::filesystem::temp_directory_path() / "oscpp_test_XXXXXX").string();
    if (mkdtemp(pattern.data()) == nullptr) {
      throw std::runtime_error("Failed to create temporary directory");
    }
    directory = pattern;
  }

  TempDirectory(const TempDirectory &) = delete;
  TempDirectory &operator=(const TempDirectory &) = delete;

  ~TempDirectory() {
    std::error_code ignored;
    std::filesystem::remove_all(directory, ignored);
  }

  /// Full path of a file with the given name inside the directory.
  [[nodiscard]] auto path(const std::string &name) const -> std::string { return (directory / name).string(); }
};

#endif // OSCPP_TEST_TEMP_DIRECTORY_HPP
