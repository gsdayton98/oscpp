// -*- mode: c++ -*-;
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.
#include <string>
#include <system_error>
#include "oscpp/sysexception.hpp"


auto oscpp::SysException::message(const int errorNumber) -> std::string {
  return std::generic_category().message(errorNumber);
}
