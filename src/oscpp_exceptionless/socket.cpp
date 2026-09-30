// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "oscpp_exceptionless/socket.hpp"

oscpp_exceptionless::Socket::Socket(Socket &&other) noexcept
: handle {other.handle}
{
  other.handle = -1;
}

oscpp_exceptionless::Socket::~Socket() noexcept {
  if (-1 < handle) {
    (void) ::close(handle);
  }
  handle = -1;
}

auto oscpp_exceptionless::Socket::create(const int domain, const int socketType, const int protocol) noexcept
    -> std::expected<Socket, std::error_code> {
#ifdef SOCK_CLOEXEC
  const int newHandle = socket(domain, socketType | SOCK_CLOEXEC, protocol);
  if (newHandle < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
#else
  // No atomic option (e.g. macOS), so there is a small window before FD_CLOEXEC is set.
  const int newHandle = socket(domain, socketType, protocol);
  if (newHandle < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  if (fcntl(newHandle, F_SETFD, FD_CLOEXEC) < 0) {
    const int error = errno;
    (void) ::close(newHandle);
    return std::unexpected(std::error_code(error, std::generic_category()));
  }
#endif
  return Socket {newHandle};
}

auto oscpp_exceptionless::Socket::clone() const noexcept -> std::expected<Socket, std::error_code> {
  const int newHandle = fcntl(handle, F_DUPFD_CLOEXEC, 0);
  if (newHandle < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return Socket {newHandle};
}
