// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/13/23.
//

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "socket.hpp"
#include "sysexception.hpp"


[[maybe_unused]] oscpp::Socket::Socket(Socket&& other) noexcept
: handle {other.handle}
{
    other.handle = -1;
}

oscpp::Socket::~Socket() noexcept
{
    if (-1 < handle) {
        (void) close(handle);
    }
    handle = -1;
}


 auto oscpp::Socket::create(const int domain, const int socketType, const int protocol) noexcept -> std::pair<Socket, int> {
    int error = 0;
#ifdef SOCK_CLOEXEC
    const int newHandle = socket(domain, socketType | SOCK_CLOEXEC, protocol);
    if (newHandle < 0) {
        error = errno;
    }
#else
    // No atomic option (e.g. macOS), so there is a small window before FD_CLOEXEC is set.
    int newHandle = socket(domain, socketType, protocol);
    if (newHandle < 0) {
        error = errno;
    } else if (fcntl(newHandle, F_SETFD, FD_CLOEXEC) < 0) {
        error = errno;
        (void) close(newHandle);
        newHandle = -1;
    }
#endif
    return std::make_pair(Socket(newHandle), error);
}


[[maybe_unused]] auto oscpp::Socket::clone() const noexcept -> std::pair<Socket, int> {
    int error = 0;
    const int newHandle = fcntl(handle, F_DUPFD_CLOEXEC, 0);
    if (newHandle < 0) {
        error = errno;
    }
    return std::make_pair(Socket(newHandle), error);
}