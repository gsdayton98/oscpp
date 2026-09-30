// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026. Glen S. Dayton. Rights reserved according to included license.
//
// Non-throwing counterpart of oscpp::RandomDevice. Construction and each draw report failure as a std::error_code
// inside a std::expected. Because a draw can fail, this is deliberately not a std::uniform_random_bit_generator;
// use oscpp::RandomDevice with the standard distributions and std::shuffle.

#ifndef OSCPP_EXCEPTIONLESS_RANDOM_DEVICE_HPP
#define OSCPP_EXCEPTIONLESS_RANDOM_DEVICE_HPP
#include <expected>
#include <memory>
#include <random>
#include <string>
#include <system_error>

namespace oscpp_exceptionless {

class __attribute__((visibility("default"))) RandomDevice {
 public:
  using result_type = std::random_device::result_type;

  /// Use the implementation's default entropy source.
  [[nodiscard]] static auto create() noexcept -> std::expected<RandomDevice, std::error_code>;

  /// Use the entropy source named by an implementation-defined token (for example "/dev/urandom").
  [[nodiscard]] static auto create(const std::string &token) noexcept -> std::expected<RandomDevice, std::error_code>;

  RandomDevice(const RandomDevice &) = delete;
  RandomDevice &operator=(const RandomDevice &) = delete;
  RandomDevice(RandomDevice &&) noexcept;
  RandomDevice &operator=(RandomDevice &&) = delete;
  ~RandomDevice() noexcept;

  /// Draw a random value.
  [[nodiscard]] auto next() noexcept -> std::expected<result_type, std::error_code>;

  [[nodiscard]] auto entropy() const noexcept -> double;

  static constexpr auto min() -> result_type { return std::random_device::min(); }

  static constexpr auto max() -> result_type { return std::random_device::max(); }

 private:
  explicit RandomDevice(std::unique_ptr<std::random_device> device) noexcept;

  std::unique_ptr<std::random_device> device;
};

}
#endif // OSCPP_EXCEPTIONLESS_RANDOM_DEVICE_HPP
