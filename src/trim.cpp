// Copyright 2016.  Glen S. Dayton.  All rights reserved.
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
