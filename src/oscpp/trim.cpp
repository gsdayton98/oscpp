// -*- mode: c++ -*-;
// Copyright 2016.  Glen S. Dayton. Rights reserved according to included license.
#include <string>
#include "oscpp/trim.hpp"

using std::isspace;
using std::string;

void oscpp::trim(string& s) {
    // Pop characters from the right if they're NUL or some sort of space.
    // isspace() is undefined for negative values, so pass the character as an unsigned char.
    while (!s.empty() && (s.back() == 0 || isspace(static_cast<unsigned char>(s.back())))) {
      s.pop_back();
    }
}
