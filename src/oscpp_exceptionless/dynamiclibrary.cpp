// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <dlfcn.h>
#include "oscpp_exceptionless/dynamiclibrary.hpp"

namespace {
auto dlError() -> std::string {
  const char *message = dlerror();
  return message == nullptr ? "Unknown DynamicLibrary error" : message;
}
}

auto oscpp_exceptionless::DynamicLibrary::create(const int flags) -> std::expected<DynamicLibrary, std::string> {
  void *handle = dlopen(nullptr, flags);
  if (handle == nullptr) return std::unexpected(dlError());
  return DynamicLibrary {handle};
}

auto oscpp_exceptionless::DynamicLibrary::create(const char *path, const int flags) -> std::expected<DynamicLibrary, std::string> {
  void *handle = dlopen(path, flags);
  if (handle == nullptr) return std::unexpected(dlError());
  return DynamicLibrary {handle};
}

oscpp_exceptionless::DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
: handle {other.handle}
{
  other.handle = nullptr;
}

oscpp_exceptionless::DynamicLibrary::~DynamicLibrary() noexcept {
  if (handle != nullptr) {
    (void) dlclose(handle);
  }
}

auto oscpp_exceptionless::DynamicLibrary::symbol(const char *symbolName) const -> std::expected<void *, std::string> {
  void *result = dlsym(handle, symbolName);
  if (result == nullptr) return std::unexpected(dlError());
  return result;
}
