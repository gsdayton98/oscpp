// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
////
///! Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
//! Created by Glen Dayton on 8/13/23.
//!
//! Provides a C++ wrapper aruound a POSIX socket.

#ifndef OSCPP_SOCKET_HPP
#define OSCPP_SOCKET_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <utility>
#include "oscpp_exceptionless/socket.hpp"

namespace oscpp {
    class __attribute__((visibility("default"))) Socket {
        oscpp_exceptionless::Socket impl;

    private:
        explicit Socket(oscpp_exceptionless::Socket &&socket) noexcept : impl{std::move(socket)} {}

    public:
        Socket(const Socket &) = delete;

        Socket(Socket &&) noexcept = default;

        ~Socket() noexcept = default;

        Socket &operator=(const Socket &) = delete;

        /// Create a socket. The descriptor is close-on-exec.
        /// @throws oscpp::SysException on failure. See oscpp_exceptionless::Socket::create for a non-throwing version.
        static auto create(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 0) -> Socket;

        /// Duplicate the socket descriptor. The duplicate is close-on-exec.
        /// @throws oscpp::SysException on failure. See oscpp_exceptionless::Socket::clone for a non-throwing version.
        [[nodiscard]] auto clone() const -> Socket;

        [[nodiscard]] int descriptor() const noexcept { return impl.descriptor(); }
    };
}
#endif //OSCPP_SOCKET_HPP
