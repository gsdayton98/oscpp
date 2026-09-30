// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "oscpp_exceptionless/file_descriptor.hpp"

oscpp_exceptionless::FileDescriptor::FileDescriptor(FileDescriptor &&original) noexcept
: handle {original.handle}
{
  original.handle = -1;
}

oscpp_exceptionless::FileDescriptor::~FileDescriptor() noexcept {
  if (-1 < handle) {
    (void) ::close(handle);
  }
}

auto oscpp_exceptionless::FileDescriptor::create(const int descriptor) noexcept -> FileDescriptor {
  return FileDescriptor {descriptor};
}

auto oscpp_exceptionless::FileDescriptor::clone() const noexcept -> std::expected<FileDescriptor, std::error_code> {
  const int newDescriptor = fcntl(handle, F_DUPFD_CLOEXEC, 0);
  if (newDescriptor < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return FileDescriptor {newDescriptor};
}
