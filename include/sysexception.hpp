// Copyright 2016. Glen S. Dayton.  All rights reserved.
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
