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
namespace oscpp {
    class __attribute__((visibility("default"))) RandomDevice {
        std::random_device r;

    public:
        /**
         * @throws std::system_error if the underlying std::random_device cannot be constructed
         * (e.g. "/dev/random" cannot be opened).
         */
        RandomDevice() : r{"/dev/random"} {}

        auto operator()() -> unsigned int { return r(); }

        [[nodiscard]] auto entropy() const -> double;

        static constexpr auto min() -> unsigned int { return std::random_device::min(); }

        static constexpr auto max() -> unsigned int { return std::random_device::max(); }
    };
}
#endif //OSCPP_RANDOM_DEVICE_HPP
