// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
//
// Non-throwing counterpart of oscpp::Socket. A Socket owns a FileDescriptor (composition, not inheritance) and
// adds the socket-specific operations. Failures are returned as a std::error_code (errno in the generic
// category) inside a std::expected.

#ifndef EXCEPTIONLESS_SOCKET_HPP
#define EXCEPTIONLESS_SOCKET_HPP
#include <expected>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"

namespace oscpp_exceptionless {

class __attribute__((visibility("default"))) Socket {
  FileDescriptor fd;

  explicit Socket(FileDescriptor &&descriptor) noexcept : fd {std::move(descriptor)} {}

 public:
  Socket(const Socket &) = delete;
  Socket(Socket &&) noexcept = default;
  ~Socket() noexcept = default;
  Socket &operator=(const Socket &) = delete;

  /// Create a socket. The descriptor is close-on-exec.
  [[nodiscard]] static auto create(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 0) noexcept
      -> std::expected<Socket, std::error_code>;

  /// Duplicate the socket descriptor. The duplicate is close-on-exec.
  [[nodiscard]] auto clone() const noexcept -> std::expected<Socket, std::error_code>;

  [[nodiscard]] int descriptor() const noexcept { return fd.descriptor(); }

  /// The underlying descriptor, for operations that work on any descriptor. The socket keeps ownership.
  [[nodiscard]] auto fileDescriptor() const noexcept -> const FileDescriptor & { return fd; }
};

}
#endif // EXCEPTIONLESS_SOCKET_HPP
