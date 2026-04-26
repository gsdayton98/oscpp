// Copyright 2016. Glen S. Dayton.  All rights reserved.
#include <cstring>
#include <string>
#include "sysexception.hpp"


auto oscpp::SysException::message(const int errorNumber) -> std::string {
  constexpr size_t MESSAGE_BUFFER_SIZE = 256;
  char errorMessage[MESSAGE_BUFFER_SIZE] = {};

  (void) strerror_r(errorNumber, errorMessage, MESSAGE_BUFFER_SIZE - 1);
  return std::string{errorMessage};
}