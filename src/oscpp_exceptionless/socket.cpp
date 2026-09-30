// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "oscpp_exceptionless/socket.hpp"

auto oscpp_exceptionless::Socket::create(const int domain, const int socketType, const int protocol) noexcept
    -> std::expected<Socket, std::error_code> {
#ifdef SOCK_CLOEXEC
  const int newHandle = socket(domain, socketType | SOCK_CLOEXEC, protocol);
  if (newHandle < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return Socket{FileDescriptor::create(newHandle)};
#else
  // No atomic option (e.g. macOS), so there is a small window before FD_CLOEXEC is set.
  // The FileDescriptor closes the handle if setting FD_CLOEXEC fails.
  const int newHandle = socket(domain, socketType, protocol);
  if (newHandle < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  Socket result{FileDescriptor::create(newHandle)};
  if (fcntl(newHandle, F_SETFD, FD_CLOEXEC) < 0) {
    return std::unexpected(std::error_code(errno, std::generic_category()));
  }
  return result;
#endif
}

auto oscpp_exceptionless::Socket::clone() const noexcept -> std::expected<Socket, std::error_code> {
  auto duplicate = fd.clone();
  if (!duplicate) {
    return std::unexpected(duplicate.error());
  }
  return Socket{std::move(*duplicate)};
}
