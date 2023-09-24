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


oscpp::Socket::Socket(int domain, int socketType,  int protocol)
: handle {-1}
{
    auto result = Socket::create(domain, socketType, protocol);
    if (result.second != 0) {
        throw SysException(errno);
    }
    handle = result.first;
}


[[maybe_unused]] oscpp::Socket::Socket(Socket&& other) noexcept
: handle {other.handle}
{
    other.handle = -1;
}

oscpp::Socket::~Socket()
{
    if (-1 < handle) {
        (void) ::close(handle);
    }
    handle = -1;
}


std::pair<int, int> oscpp::Socket::create(int domain, int socketType, int protocol) {
    int error = 0;
    int newHandle = ::socket(domain, socketType, protocol);
    if (newHandle < 0) {
        error = errno;
    }
    return std::make_pair(newHandle, error);
}


[[maybe_unused]] Socket oscpp::Socket::clone() const {
    int newHandle = ::dup(handle);
    return Socket(newHandle);
}