// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cstring>
#include <dlfcn.h>
#include "oscpp_exceptionless/dynamiclibrary.hpp"

auto oscpp_exceptionless::DynamicLibraryError::fromText(const char *text) noexcept -> DynamicLibraryError {
  DynamicLibraryError error {};
  if (text != nullptr) {
    std::strncpy(error.message, text, Capacity - 1);
  }
  return error;
}

auto oscpp_exceptionless::DynamicLibraryError::fromDlerror() noexcept -> DynamicLibraryError {
  return fromText(dlerror());
}

namespace {
auto dlError() noexcept -> oscpp_exceptionless::DynamicLibraryError {
  return oscpp_exceptionless::DynamicLibraryError::fromDlerror();
}
}

auto oscpp_exceptionless::DynamicLibrary::create(const int flags) noexcept -> std::expected<DynamicLibrary, DynamicLibraryError> {
  void *handle = dlopen(nullptr, flags);
  if (handle == nullptr) return std::unexpected(dlError());
  return DynamicLibrary {handle};
}

auto oscpp_exceptionless::DynamicLibrary::create(const char *path, const int flags) noexcept -> std::expected<DynamicLibrary, DynamicLibraryError> {
  void *handle = dlopen(path, flags);
  if (handle == nullptr) return std::unexpected(dlError());
  return DynamicLibrary {handle};
}

oscpp_exceptionless::DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
: handle {other.handle}
{
  other.handle = nullptr;
}

auto oscpp_exceptionless::DynamicLibrary::operator=(DynamicLibrary &&other) noexcept -> DynamicLibrary & {
  if (this != &other) {
    if (handle != nullptr) {
      (void) dlclose(handle);
    }
    handle = other.handle;
    other.handle = nullptr;
  }
  return *this;
}

oscpp_exceptionless::DynamicLibrary::~DynamicLibrary() noexcept {
  if (handle != nullptr) {
    (void) dlclose(handle);
  }
}

auto oscpp_exceptionless::DynamicLibrary::symbol(const char *symbolName) const noexcept -> std::expected<void *, DynamicLibraryError> {
  void *result = dlsym(handle, symbolName);
  if (result == nullptr) return std::unexpected(dlError());
  return result;
}
