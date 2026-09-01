// Copyright 2016.  Glen S. Dayton. Rights reserved according to included license.
#ifndef OSCPP_TRIM_HPP
#define OSCPP_TRIM_HPP
#include <string>

namespace oscpp {

/**
 *  Trim trailing whitespace from the end of a string.
 *
 * @param s      String to trim.
 */
    [[maybe_unused]] __attribute__((visibility("default")))
    void trim(std::string &s);
}
#endif