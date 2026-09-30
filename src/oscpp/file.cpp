// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include "oscpp/file.hpp"

namespace {
auto openOrThrow(const char *filename, const int flags, const int mode) -> oscpp_exceptionless::File {
  auto result = oscpp_exceptionless::File::create(filename, flags, mode);
  if (!result)
    throw oscpp::SysException{result.error()};
  return std::move(*result);
}
} // namespace

oscpp::File::File(const char *filename, const int flags, const int mode) : impl{openOrThrow(filename, flags, mode)} {}

std::pair<void *, std::size_t> oscpp::File::map() {
  const auto result = impl.map();
  if (!result)
    throw SysException{result.error()};
  return *result;
}

struct stat &oscpp::File::fstat(struct stat &buffer) const {
  const auto result = impl.fstat();
  if (!result)
    throw SysException{result.error()};
  buffer = *result;
  return buffer;
}
