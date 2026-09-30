// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016. Glen S. Dayton. Rights reserved according to included license.
////
//! SysException
//!
//! Using the last POSIX errno, extract its associated error message in a std::runtime_error, suitable for
//! throwing as an exception, or returning as a status value.
#ifndef OSCPP_SYSEXCEPTION_HPP
#define OSCPP_SYSEXCEPTION_HPP
#include <cerrno>
#include <stdexcept>
#include <string>
namespace oscpp {
    class __attribute__((visibility("default"))) SysException : public std::runtime_error {
    public:
        explicit SysException(const int errorNumber = errno)
        : std::runtime_error(message(errorNumber))
        {}

        static auto message(int errorNumber) -> std::string;
    };
}
#endif