// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 7/17/23.
//
// RandomDevice
//
// Implementation specific wrapper around C++ random_device.
// C++ random_device may block.

#ifndef OSCPP_RANDOM_DEVICE_HPP
#define OSCPP_RANDOM_DEVICE_HPP

#include <random>
#include <string>
#include <system_error>
#include "oscpp_exceptionless/random_device.hpp"
namespace oscpp {
/**
 * Satisfies std::uniform_random_bit_generator, so it can be used with the standard distributions and
 * std::shuffle. See oscpp_exceptionless::RandomDevice for a non-throwing version.
 */
class __attribute__((visibility("default"))) RandomDevice {
  oscpp_exceptionless::RandomDevice impl;

 public:
  using result_type = oscpp_exceptionless::RandomDevice::result_type;

  /**
   * Use the implementation's default entropy source.
   * @throws std::system_error if the underlying std::random_device cannot be constructed.
   */
  RandomDevice();

  /**
   * Use the entropy source named by an implementation-defined token (for example "/dev/urandom").
   * @throws std::system_error if the underlying std::random_device cannot be constructed
   * (behavior for unrecognized tokens is implementation-defined).
   */
  explicit RandomDevice(const std::string &token);

  /// @throws std::system_error if the device fails to produce a value.
  auto operator()() -> result_type;

  [[nodiscard]] auto entropy() const -> double;

  static constexpr auto min() -> result_type { return oscpp_exceptionless::RandomDevice::min(); }

  static constexpr auto max() -> result_type { return oscpp_exceptionless::RandomDevice::max(); }
};

static_assert(std::uniform_random_bit_generator<RandomDevice>);
} // namespace oscpp
#endif // OSCPP_RANDOM_DEVICE_HPP
