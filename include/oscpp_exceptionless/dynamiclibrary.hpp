// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Non-throwing counterpart of oscpp::DynamicLibrary. Failures are returned as a DynamicLibraryError inside a
// std::expected. Nothing here throws or allocates.
//
// The error holds the dlerror() text rather than a std::error_code because dlerror() reports text, not errno. It is a
// fixed-size buffer rather than a std::string because building or copying a std::string can throw std::bad_alloc.

#ifndef OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
#define OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
#include <dlfcn.h>
#include <cstddef>
#include <expected>
#include <type_traits>

namespace oscpp_exceptionless {

/// A dlerror() message: fixed-size, trivially copyable, and never allocates. Text longer than the buffer is truncated.
struct __attribute__((visibility("default"))) DynamicLibraryError {
  static constexpr std::size_t Capacity = 256;

  char message[Capacity];

  /// Copy `text` (truncating it if necessary). A null pointer gives an empty message; that is what dlerror() returns
  /// when no error has occurred since its last call.
  [[nodiscard]] static auto fromText(const char *text) noexcept -> DynamicLibraryError;

  /// Take the message from dlerror(), which clears its error state. Empty if there was no pending error.
  [[nodiscard]] static auto fromDlerror() noexcept -> DynamicLibraryError;

  /// The NUL-terminated message.
  [[nodiscard]] auto what() const noexcept -> const char * { return message; }
};

static_assert(std::is_trivially_copyable_v<DynamicLibraryError>);

class __attribute__((visibility("default"))) DynamicLibrary {
 public:
  /// Flags passed to dlopen() unless the caller chooses others.
  static constexpr int DefaultFlags = RTLD_NOW | RTLD_LOCAL;

  /// Open the current application image.
  [[nodiscard]] static auto create(int flags = DefaultFlags) noexcept
      -> std::expected<DynamicLibrary, DynamicLibraryError>;

  /// Load the specified path into the image. @param flags dlopen() flags (RTLD_LAZY or RTLD_NOW, optionally
  /// RTLD_GLOBAL...).
  [[nodiscard]] static auto create(const char *path, int flags = DefaultFlags) noexcept
      -> std::expected<DynamicLibrary, DynamicLibraryError>;

  DynamicLibrary(const DynamicLibrary &) = delete;
  DynamicLibrary &operator=(const DynamicLibrary &) = delete;
  DynamicLibrary(DynamicLibrary &&other) noexcept;
  /// Close the library this object holds (if any), then take ownership of the other's.
  DynamicLibrary &operator=(DynamicLibrary &&other) noexcept;

  /// Close the library.
  ~DynamicLibrary() noexcept;

  /// Find the specified symbol in the currently open library.
  [[nodiscard]] auto symbol(const char *symbolName) const noexcept -> std::expected<void *, DynamicLibraryError>;

 private:
  explicit DynamicLibrary(void *libraryHandle) noexcept : handle{libraryHandle} {}

  void *handle;
};

} // namespace oscpp_exceptionless
#endif // OSCPP_EXCEPTIONLESS_DYNAMIC_LIBRARY_HPP
