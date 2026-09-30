// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <string>
#include <system_error>
#include "oscpp/sysexception.hpp"

auto oscpp::SysException::message(const int errorNumber) -> std::string {
  return std::generic_category().message(errorNumber);
}
