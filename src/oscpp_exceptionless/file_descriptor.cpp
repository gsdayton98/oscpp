// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"

oscpp_exceptionless::FileDescriptor::FileDescriptor(FileDescriptor &&original) noexcept : handle{original.handle} {
  original.handle = -1;
}

oscpp_exceptionless::FileDescriptor::~FileDescriptor() noexcept {
  if (-1 < handle) {
    (void)::close(handle);
  }
}

auto oscpp_exceptionless::FileDescriptor::create(const int descriptor) noexcept -> FileDescriptor {
  return FileDescriptor{descriptor};
}

auto oscpp_exceptionless::FileDescriptor::clone() const noexcept -> std::expected<FileDescriptor, std::error_code> {
  const int newDescriptor = fcntl(handle, F_DUPFD_CLOEXEC, 0);
  if (newDescriptor < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return FileDescriptor{newDescriptor};
}

auto oscpp_exceptionless::FileDescriptor::operator=(FileDescriptor &&other) noexcept -> FileDescriptor & {
  if (this != &other) {
    if (-1 < handle) {
      (void)::close(handle);
    }
    handle = other.handle;
    other.handle = -1;
  }
  return *this;
}

auto oscpp_exceptionless::FileDescriptor::release() noexcept -> int {
  return std::exchange(handle, -1);
}

auto oscpp_exceptionless::FileDescriptor::read(void *buffer, const std::size_t length) noexcept
    -> std::expected<std::size_t, std::error_code> {
  const ssize_t count = ::read(handle, buffer, length);
  if (count < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return static_cast<std::size_t>(count);
}

auto oscpp_exceptionless::FileDescriptor::write(const void *buffer, const std::size_t length) noexcept
    -> std::expected<std::size_t, std::error_code> {
  const ssize_t count = ::write(handle, buffer, length);
  if (count < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return static_cast<std::size_t>(count);
}
