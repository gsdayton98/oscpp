// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
////
// Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
// Created by Glen Dayton on 7/17/23.
//
//  RandomDevice
//
//  Implementation specific wrapper around C++ random_device.
//  C++ random_device may block.

#ifndef OSCPP_RANDOM_DEVICE_HPP
#define OSCPP_RANDOM_DEVICE_HPP

#include <random>
#include <string>
namespace oscpp {
    /**
     * Satisfies std::uniform_random_bit_generator, so it can be used with the standard distributions and
     * std::shuffle.
     */
    class __attribute__((visibility("default"))) RandomDevice {
        std::random_device r;

    public:
        using result_type = std::random_device::result_type;

        /**
         * Use the implementation's default entropy source.
         * @throws std::system_error if the underlying std::random_device cannot be constructed.
         */
        RandomDevice() : r{} {}

        /**
         * Use the entropy source named by an implementation-defined token (for example "/dev/urandom").
         * @throws std::system_error if the underlying std::random_device cannot be constructed
         * (behavior for unrecognized tokens is implementation-defined).
         */
        explicit RandomDevice(const std::string &token) : r{token} {}

        auto operator()() -> result_type { return r(); }

        [[nodiscard]] auto entropy() const -> double;

        static constexpr auto min() -> result_type { return std::random_device::min(); }

        static constexpr auto max() -> result_type { return std::random_device::max(); }
    };

    static_assert(std::uniform_random_bit_generator<RandomDevice>);
}
#endif //OSCPP_RANDOM_DEVICE_HPP
