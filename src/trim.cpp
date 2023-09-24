// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.
#include <cctype>
#include <string>
#include <trim.hpp>

using std::isspace;
using std::string;

void oscpp::trim(string& s) {
    while (s.length() > 0 && (s.back() == 0 || isspace(s.back()))) {
      s.pop_back();
    }
}
