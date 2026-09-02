// -*- mode: c++ -*-;
// Copyright 2016.  Glen S. Dayton. Rights reserved according to included license.
#include <string>
#include <trim.hpp>

using std::isspace;
using std::string;

void oscpp::trim(string& s) {
    while (!s.empty() && (s.back() == 0 || isspace(s.back()))) {
      s.pop_back();
    }
}
