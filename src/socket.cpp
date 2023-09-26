// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/13/23.
//

#include <cerrno>
#include <unistd.h>
#include "socket.hpp"
#include "sysexception.hpp"

using namespace oscpp;

[[maybe_unused]] oscpp::Socket::Socket(Socket&& other) noexcept
: handle {other.handle}
{
    other.handle = -1;
}

oscpp::Socket::~Socket() noexcept
{
    if (-1 < handle) {
        (void) ::close(handle);
    }
    handle = -1;
}


 auto oscpp::Socket::create(int domain, int socketType, int protocol) noexcept -> std::pair<Socket, int> {
    int error = 0;
    int newHandle = ::socket(domain, socketType, protocol);
    if (newHandle < 0) {
        error = errno;
    }
    return std::make_pair(Socket(newHandle), error);
}


[[maybe_unused]] auto oscpp::Socket::clone() const noexcept -> std::pair<Socket, int> {
    int error = 0;
    int newHandle = ::dup(handle);
    if (newHandle < 0) {
        error = errno;
    }
    return std::make_pair(Socket(newHandle), error);
}