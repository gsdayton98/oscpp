// -*- mode: c++ -*-
// @copyright  2021 Glen S. Dayton. Rights reserved according to terms of included license.
//  @author Glen S.Dayton

#include "oscpp/file.hpp"

namespace {
auto openOrThrow(const char *filename, const int flags, const int mode) -> oscpp_exceptionless::File {
  auto result = oscpp_exceptionless::File::create(filename, flags, mode);
  if (!result) throw oscpp::SysException{result.error()};
  return std::move(*result);
}
}

oscpp::File::File(const char *filename, const int flags, const int mode)
: impl {openOrThrow(filename, flags, mode)}
{}


std::pair<void*, std::size_t> oscpp::File::map() {
  const auto result = impl.map();
  if (!result) throw SysException{result.error()};
  return *result;
}


struct stat& oscpp::File::fstat(struct stat& buffer) const {
  const auto result = impl.fstat();
  if (!result) throw SysException{result.error()};
  buffer = *result;
  return buffer;
}
