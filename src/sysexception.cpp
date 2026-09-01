// Copyright 2016. Glen S. Dayton.  All rights reserved.
#include <cstring>
#include <string>
#include "sysexception.hpp"


auto oscpp::SysException::message(const int errorNumber) -> std::string {
  constexpr size_t MESSAGE_BUFFER_SIZE = 256;
  char errorMessage[MESSAGE_BUFFER_SIZE] = {};

  switch (strerror_r(errorNumber, errorMessage, MESSAGE_BUFFER_SIZE - 1))
  {
    // On success or truncated message, use the returned message.
    case 0:
    case ERANGE:
    case EINVAL:
    break;

    default:
    strncpy(errorMessage, "Unable to determine message", sizeof(errorMessage) );
  }
  return std::string{errorMessage};
}