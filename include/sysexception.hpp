// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cerrno>
#include <stdexcept>
#include <string>

namespace oscpp {
    class SysException : public std::runtime_error {
    public:
        explicit SysException(int errorNumber = errno)
        : std::runtime_error(SysException::message(errorNumber))
        {}

        static std::string message(int errorNumber);
    };
}
