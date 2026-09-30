// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <exception>
#include <new>
#include <string>
#include "oscpp_exceptionless/random_device.hpp"

namespace {
// Translate whatever std::random_device threw into an error code.
auto currentError() noexcept -> std::error_code {
  try {
    throw;
  } catch (const std::system_error &ex) {
    return ex.code();
  } catch (const std::bad_alloc &) {
    return std::make_error_code(std::errc::not_enough_memory);
  } catch (...) {
    return std::make_error_code(std::errc::io_error);
  }
}
}

oscpp_exceptionless::RandomDevice::RandomDevice(std::unique_ptr<std::random_device> d) noexcept
: device {std::move(d)}
{}

oscpp_exceptionless::RandomDevice::RandomDevice(RandomDevice &&) noexcept = default;
oscpp_exceptionless::RandomDevice::~RandomDevice() noexcept = default;

auto oscpp_exceptionless::RandomDevice::create() noexcept -> std::expected<RandomDevice, std::error_code> {
  try {
    return RandomDevice {std::make_unique<std::random_device>()};
  } catch (...) {
    return std::unexpected(currentError());
  }
}

auto oscpp_exceptionless::RandomDevice::create(const char *token) noexcept
    -> std::expected<RandomDevice, std::error_code> {
  try {
    return RandomDevice {std::make_unique<std::random_device>(std::string{token})};
  } catch (...) {
    return std::unexpected(currentError());
  }
}

auto oscpp_exceptionless::RandomDevice::next() noexcept -> std::expected<result_type, std::error_code> {
  try {
    return (*device)();
  } catch (...) {
    return std::unexpected(currentError());
  }
}

auto oscpp_exceptionless::RandomDevice::entropy() const noexcept -> double { return device->entropy(); }
