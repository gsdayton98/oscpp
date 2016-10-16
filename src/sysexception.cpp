// Copyright 2016. Glen S. Dayton.  All rights reserved.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sysexception.hpp>

using std::size_t;
using std::string;


string oscpp::SysException::message(int errorNumber) {
  static const size_t MESSAGEBUFFERSIZE = 256;
  string errorMessage(MESSAGEBUFFERSIZE, 0);

  (void) ::strerror_r(errorNumber, &errorMessage[0], errorMessage.size() - 1);
  return errorMessage;
}
