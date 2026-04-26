// Copyright 2016.  Glen S. Dayton.  All rights reserved.
#ifndef TRIM_HPP
#define TRIM_HPP
#include <string>

namespace oscpp {

/**
 *  Trim trailing whitespace from the end of a string.
 *
 * @param s      String to trim.
 */
    [[maybe_unused]]
    void trim(std::string &s);
}
#endif