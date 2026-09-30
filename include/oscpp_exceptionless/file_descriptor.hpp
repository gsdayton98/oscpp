// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to the included license terms.
//
// Non-throwing counterpart of oscpp::FileDescriptor. Same semantics (non-copyable, movable, closes on destruction),
// but failures are returned as a std::error_code (errno in the generic category) inside a std::expected.

#ifndef EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
#define EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
#include <expected>
#include <system_error>

namespace oscpp_exceptionless {

class __attribute__((visibility("default"))) FileDescriptor {
  int handle;

  explicit FileDescriptor(const int fileDescriptor) noexcept : handle {fileDescriptor} {}

 public:
  /// Take ownership of an existing system descriptor.
  [[nodiscard]] static auto create(int descriptor) noexcept -> FileDescriptor;

  FileDescriptor(const FileDescriptor &) = delete;
  FileDescriptor(FileDescriptor &&) noexcept;
  ~FileDescriptor() noexcept;
  FileDescriptor &operator=(const FileDescriptor &) = delete;
  FileDescriptor &operator=(FileDescriptor &&) = delete;

  /// Duplicate the descriptor. The duplicate is close-on-exec.
  [[nodiscard]] auto clone() const noexcept -> std::expected<FileDescriptor, std::error_code>;

  [[nodiscard]] auto descriptor() const noexcept -> int { return handle; }
};

}
#endif // EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
