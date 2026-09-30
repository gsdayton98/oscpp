// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
//
// Created by Glen Dayton on 7/17/23.
//

#include "oscpp/random_device.hpp"

namespace {
auto unwrap(std::expected<oscpp_exceptionless::RandomDevice, std::error_code> &&result)
    -> oscpp_exceptionless::RandomDevice {
    if (!result) throw std::system_error(result.error());
    return std::move(*result);
}
}

oscpp::RandomDevice::RandomDevice() : impl{unwrap(oscpp_exceptionless::RandomDevice::create())} {}

oscpp::RandomDevice::RandomDevice(const std::string &token)
    : impl{unwrap(oscpp_exceptionless::RandomDevice::create(token))} {}

auto oscpp::RandomDevice::operator()() -> result_type {
    const auto value = impl.next();
    if (!value) throw std::system_error(value.error());
    return *value;
}

auto oscpp::RandomDevice::entropy() const -> double { return impl.entropy(); }
