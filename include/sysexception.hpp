// Copyright 2016. Glen S. Dayton. Rights reserved according to included license.
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