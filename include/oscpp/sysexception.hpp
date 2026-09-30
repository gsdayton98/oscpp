// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// SysException
//
// Using the last POSIX errno, extract its associated error message in a std::system_error, suitable for
// throwing as an exception, or returning as a status value. Callers can inspect code() to branch on the error.

#ifndef OSCPP_SYSEXCEPTION_HPP
#define OSCPP_SYSEXCEPTION_HPP
#include <cerrno>
#include <string>
#include <system_error>
#include "oscpp_export.hpp"
namespace oscpp {
class OSCPP_API SysException : public std::system_error {
 public:
  /// Construct from an errno value (default: the current errno) in the generic category.
  explicit SysException(const int errorNumber = errno)
      : SysException(std::error_code(errorNumber, std::generic_category())) {}

  /// Construct from an error code (what() is the code's message, including for the zero code), such as one returned by
  /// the oscpp_exceptionless classes.
  explicit SysException(const std::error_code errorCode)
      : std::system_error(errorCode, errorCode ? std::string{} : errorCode.message()) {}

  static auto message(int errorNumber) -> std::string;
};
} // namespace oscpp
#endif // OSCPP_SYSEXCEPTION_HPP
