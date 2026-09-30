// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Non-throwing counterpart of oscpp::DynamicLibrary. Failures are returned as the dlerror() message inside a
// std::expected. The error is a std::string rather than a std::error_code because dlerror() reports text, not errno.

#ifndef OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
#define OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
#include <expected>
#include <string>

namespace oscpp_exceptionless {

class __attribute__((visibility("default"))) DynamicLibrary {
 public:
  /// Open the current application image.
  [[nodiscard]] static auto create() -> std::expected<DynamicLibrary, std::string>;

  /// Load the specified path into the image.
  [[nodiscard]] static auto create(const char *path) -> std::expected<DynamicLibrary, std::string>;

  DynamicLibrary(const DynamicLibrary &) = delete;
  DynamicLibrary &operator=(const DynamicLibrary &) = delete;
  DynamicLibrary(DynamicLibrary &&other) noexcept;
  DynamicLibrary &operator=(DynamicLibrary &&) = delete;

  /// Close the library.
  ~DynamicLibrary() noexcept;

  /// Find the specified symbol in the currently open library.
  [[nodiscard]] auto symbol(const char *symbolName) const -> std::expected<void *, std::string>;

 private:
  explicit DynamicLibrary(void *libraryHandle) noexcept : handle {libraryHandle} {}

  void *handle;
};

}
#endif // OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
