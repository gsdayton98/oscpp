// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// OSCPP File Descriptor
// Created by Glen Dayton on 8/14/23.
//
// Captures the use semantics of a typical handle or file descriptor.  Prohibits copying but provides a clone()
// method for duplicating the descriptor.  Closes the descriptor on destruction.

#ifndef OSCPP_FILE_DESCRIPTOR_HPP
#define OSCPP_FILE_DESCRIPTOR_HPP
#include <cstddef>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"
#include "oscpp_export.hpp"

namespace oscpp {

class OSCPP_API FileDescriptor {
  /**
   * Implementation-dependent file handle or descriptor.
   */
  oscpp_exceptionless::FileDescriptor impl;

  /**
   * Use the create() method to create new file descriptors
   * @param fileDescriptor
   */
  explicit FileDescriptor(oscpp_exceptionless::FileDescriptor &&descriptor) noexcept : impl{std::move(descriptor)} {}

 public:
  static auto create(int descriptor) noexcept -> FileDescriptor;

  /**
   * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
   */
  FileDescriptor(const FileDescriptor &) = delete;

  /**
   * Move constructor creates a new FileDescriptor with the same handle and disables the old handle to prevent
   * it from getting closed.
   */
  FileDescriptor(FileDescriptor &&) noexcept = default;

  /**
   * Close the descriptor.
   */
  ~FileDescriptor() noexcept = default;

  /**
   * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
   */
  FileDescriptor &operator=(const FileDescriptor &) = delete;

  /**
   * Close the descriptor this object holds (if any), then take ownership of the other's. The source is left
   * without a descriptor.
   */
  FileDescriptor &operator=(FileDescriptor &&) noexcept = default;

  /**
   * Duplicate the existing FileDescriptor into a new FileDescriptor. The new descriptor is close-on-exec.
   * @return The new FileDescriptor.
   * @throws oscpp::SysException on failure. See oscpp_exceptionless::FileDescriptor::clone for a non-throwing version.
   */
  [[nodiscard]] auto clone() const -> FileDescriptor;

  /**
   * Return the low-level implementation specific file descriptor.
   * @return Operating system file handle
   */
  [[nodiscard]] auto descriptor() const noexcept -> int { return impl.descriptor(); }

  /// True if this object holds a descriptor (it is not moved-from or released).
  [[nodiscard]] auto valid() const noexcept -> bool { return impl.valid(); }
  explicit operator bool() const noexcept { return valid(); }

  /// Give up ownership without closing. The caller becomes responsible for closing the returned descriptor.
  [[nodiscard]] auto release() noexcept -> int { return impl.release(); }

  /**
   * Read up to `length` bytes. Like read(2), may transfer fewer bytes; 0 means end of file.
   * @return The number of bytes read.
   * @throws oscpp::SysException on failure. See oscpp_exceptionless::FileDescriptor::read for a non-throwing version.
   */
  auto read(void *buffer, std::size_t length) -> std::size_t;

  /**
   * Write up to `length` bytes. Like write(2), may transfer fewer bytes than requested.
   * @return The number of bytes written.
   * @throws oscpp::SysException on failure. See oscpp_exceptionless::FileDescriptor::write for a non-throwing version.
   */
  auto write(const void *buffer, std::size_t length) -> std::size_t;
};
} // namespace oscpp
#endif // OSCPP_FILE_DESCRIPTOR_HPP
