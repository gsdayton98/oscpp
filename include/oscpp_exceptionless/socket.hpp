// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Non-throwing counterpart of oscpp::Socket. A Socket owns a FileDescriptor (composition, not inheritance) and
// adds the socket-specific operations. Failures are returned as a std::error_code (errno in the generic
// category) inside a std::expected.

#ifndef OSCPP_EXCEPTIONLESS_SOCKET_HPP
#define OSCPP_EXCEPTIONLESS_SOCKET_HPP
#include <expected>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"
#include "oscpp_export.hpp"

namespace oscpp_exceptionless {

class OSCPP_API Socket {
  FileDescriptor fd;

  explicit Socket(FileDescriptor &&descriptor) noexcept : fd{std::move(descriptor)} {}

 public:
  Socket(const Socket &) = delete;
  Socket(Socket &&) noexcept = default;
  ~Socket() noexcept = default;
  Socket &operator=(const Socket &) = delete;

  /// Close the descriptor this socket holds (if any), then take ownership of the other's.
  Socket &operator=(Socket &&) noexcept = default;

  /// Create a socket. The descriptor is close-on-exec.
  [[nodiscard]] static auto create(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 0) noexcept
      -> std::expected<Socket, std::error_code>;

  /// Duplicate the socket descriptor. The duplicate is close-on-exec.
  [[nodiscard]] auto clone() const noexcept -> std::expected<Socket, std::error_code>;

  [[nodiscard]] int descriptor() const noexcept { return fd.descriptor(); }

  /// True if this socket holds a descriptor (it is not moved-from or released).
  [[nodiscard]] auto valid() const noexcept -> bool { return fd.valid(); }
  explicit operator bool() const noexcept { return valid(); }

  /// Give up ownership without closing. The caller becomes responsible for closing the returned descriptor.
  [[nodiscard]] auto release() noexcept -> int { return fd.release(); }

  /// The underlying descriptor, for operations that work on any descriptor. The socket keeps ownership.
  [[nodiscard]] auto fileDescriptor() const noexcept -> const FileDescriptor & { return fd; }
};

} // namespace oscpp_exceptionless
#endif // OSCPP_EXCEPTIONLESS_SOCKET_HPP
