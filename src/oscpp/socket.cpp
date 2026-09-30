// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/13/23.
//

#include "oscpp/socket.hpp"
#include "oscpp/sysexception.hpp"

auto oscpp::Socket::create(const int domain, const int socketType, const int protocol) -> Socket {
    auto result = oscpp_exceptionless::Socket::create(domain, socketType, protocol);
    if (!result) throw SysException(result.error());
    return Socket(std::move(*result));
}


auto oscpp::Socket::clone() const -> Socket {
    auto result = impl.clone();
    if (!result) throw SysException(result.error());
    return Socket(std::move(*result));
}
