# Changelog

All notable changes to oscpp are recorded here. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and the project uses [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [2.0.0] - 2026-09-30

A breaking release. The API is split into a throwing namespace (`oscpp`) and a non-throwing one
(`oscpp_exceptionless`), and headers move into directories named after the namespaces.

### Breaking changes

- **Headers moved.** Include `oscpp/<component>.hpp` (was `<component>.hpp`). They install to `include/oscpp/`, so the
  generic names `file.hpp`, `socket.hpp` and `trim.hpp` no longer land directly in the include directory.
- **`Socket::create`, `Socket::clone` and `FileDescriptor::clone` throw and return the object.** They used to return
  `std::pair<T, int>` with an errno. The non-throwing forms are now `oscpp_exceptionless::Socket::create`,
  `oscpp_exceptionless::Socket::clone` and `oscpp_exceptionless::FileDescriptor::clone`, which return
  `std::expected<T, std::error_code>`.
- **`SysException` derives from `std::system_error`** (was `std::runtime_error`), so `code()` is available. Handlers
  that catch `std::runtime_error` still work. Its `what()` text is now produced by
  `std::generic_category().message()`. A constructor taking a `std::error_code` is added.
- **`DynamicLibrary` opens with `RTLD_NOW | RTLD_LOCAL` by default** (was a literal `0`, which glibc rejects). Both
  constructors take an optional `flags` argument. Errors are still thrown as `std::runtime_error`.
- **`RandomDevice` is a `std::uniform_random_bit_generator`** with a `result_type`, and can be given an entropy-source
  token. The default source is the implementation's `std::random_device`, not a hard-coded `/dev/random`.
- **`CircularBuffer` is header-only.** The library no longer exports instantiations for common element types; any
  default-constructible, copy-assignable type works. The header no longer leaks `using std::unique_lock` and
  `using std::mutex` into the global namespace.
- **The CMake package** exports `oscpp-targets.cmake` (was `oscpp.cmake`), and version compatibility is now
  `SameMajorVersion` (was `AnyNewerVersion`), so `find_package(oscpp 1)` does not accept 2.x. Install directories come
  from `GNUInstallDirs`, so the library may land in `lib64` where the platform does that. The C++23 requirement and the
  include directory propagate through the `oscpp::oscpp` target.
- **The shared library's version is 2.0.0** with `SOVERSION` 2 (`liboscpp.2.dylib`).

### Added

- **`oscpp_exceptionless` namespace** (`include/oscpp_exceptionless/`), a non-throwing counterpart of `DynamicLibrary`,
  `File`, `FileDescriptor`, `RandomDevice` and `Socket`. Nothing in it throws or uses an allocating type such as
  `std::string` in its interface: every function is `noexcept`, and `static_assert`s check it. `DynamicLibrary` reports
  errors as a fixed-size `DynamicLibraryError`. The `oscpp` classes wrap their counterparts and turn errors into
  exceptions. Use one namespace or the other.
- **Move assignment** for `File`, `FileDescriptor`, `Socket` and `DynamicLibrary` (it closes the old resource first),
  and a move constructor for `DynamicLibrary`.
- **`valid()`, `explicit operator bool` and `release()`** on `File`, `FileDescriptor` and `Socket`.
- **`FileDescriptor::read` and `write`**, `File::descriptor()` and `File::unmap()`.
- **`OSCPP_API`** (`oscpp_export.hpp`) marks public symbols, replacing the repeated visibility attribute.
- **Build options and tooling:** `OSCPP_BUILD_TESTING` (on by default only for the top-level project),
  `OSCPP_SANITIZE`, `OSCPP_WERROR`, an `oscpp_warnings` target, `format` and `check-format` targets, an `oscpp::oscpp`
  alias, `CMakePresets.json` (`debug`, `release`, `asan`, `tsan`), GitHub Actions CI, and `.clang-format` and
  `.editorconfig`.
- **Tests:** per-case CTest discovery (`ctest -R File/Test_map`), a separate multithreaded stress test (label `stress`),
  tests that write files use a temporary directory, and broader coverage of moves, error paths and the new members.
- **`CHANGELOG.md`**, and a rewritten `README.md`.

### Fixed

- `File::map()` on an empty file no longer fails with `EINVAL`; it returns `{nullptr, 0}`. The `off_t` to `size_t`
  conversion is explicit.
- Duplicated descriptors (`clone()`) and new sockets are close-on-exec.
- `trim()` no longer passes a negative `char` to `isspace()`, which is undefined for bytes with the high bit set.
- `CircularBuffer`: the destructor definition no longer triggers `-Wdtor-name`, and `size()` no longer depends on
  operator precedence.
- The documentation for `CircularBuffer` now says its storage is heap-allocated.

### Removed

- The `std::pair<T, int>` returning forms of `Socket::create`, `Socket::clone` and `FileDescriptor::clone` (see
  `oscpp_exceptionless`).
- The meaningless `[[maybe_unused]]` on public classes and methods.
- The unused `test/mock_strerror.cpp` and `test/sampleDynamic/sampleDynamic.cpp`.

## [1.0.0]

The initial version: a flat `include/` directory, exceptions and `std::pair<T, int>` error returns mixed in one
namespace, and an `AnyNewerVersion` CMake package.
