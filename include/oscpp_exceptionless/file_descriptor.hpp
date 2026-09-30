// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to the included license terms.
//
// Non-throwing counterpart of oscpp::FileDescriptor. Same semantics (non-copyable, movable, closes on destruction),
// but failures are returned as a std::error_code (errno in the generic category) inside a std::expected.

#ifndef EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
#define EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
#include <cstddef>
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

  /// Close the descriptor this object holds (if any), then take ownership of the other's. Self-assignment is harmless.
  FileDescriptor &operator=(FileDescriptor &&other) noexcept;

  /// Duplicate the descriptor. The duplicate is close-on-exec.
  [[nodiscard]] auto clone() const noexcept -> std::expected<FileDescriptor, std::error_code>;

  [[nodiscard]] auto descriptor() const noexcept -> int { return handle; }

  /// True if this object holds a descriptor (it is not moved-from, released or created from a negative value).
  [[nodiscard]] auto valid() const noexcept -> bool { return handle >= 0; }
  explicit operator bool() const noexcept { return valid(); }

  /// Give up ownership without closing. The caller becomes responsible for closing the returned descriptor.
  [[nodiscard]] auto release() noexcept -> int;

  /// Read up to `length` bytes. Like read(2), may transfer fewer bytes; 0 means end of file.
  /// @return The number of bytes read.
  [[nodiscard]] auto read(void *buffer, std::size_t length) noexcept -> std::expected<std::size_t, std::error_code>;

  /// Write up to `length` bytes. Like write(2), may transfer fewer bytes than requested.
  /// @return The number of bytes written.
  [[nodiscard]] auto write(const void *buffer, std::size_t length) noexcept
      -> std::expected<std::size_t, std::error_code>;
};

}
#endif // EXCEPTIONLESS_FILE_DESCRIPTOR_HPP
