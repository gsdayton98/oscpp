// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Uses both APIs from an installed oscpp: the throwing oscpp::Socket and the non-throwing oscpp_exceptionless::File.

#include <iostream>
#include "oscpp/socket.hpp"
#include "oscpp_exceptionless/file.hpp"

int main() {
  const auto socket = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
  if (!socket) {
    std::cerr << "oscpp::Socket is not valid\n";
    return 1;
  }

  const auto missing = oscpp_exceptionless::File::create("/nonexistent/oscpp/package/check");
  if (missing || missing.error() != std::errc::no_such_file_or_directory) {
    std::cerr << "expected ENOENT from oscpp_exceptionless::File\n";
    return 1;
  }

  std::cout << "oscpp package OK\n";
  return 0;
}
