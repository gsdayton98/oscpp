// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#ifndef OSCPP_TRIM_HPP
#define OSCPP_TRIM_HPP
#include <string>

namespace oscpp {

/**
 *  Trim trailing whitespace from the end of a string.
 *
 * @param s      String to trim.
 */
__attribute__((visibility("default"))) void trim(std::string &s);
} // namespace oscpp
#endif // OSCPP_TRIM_HPP
