# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

oscpp is a C++23 header/source library that wraps POSIX/OS-specific facilities (file descriptors, files, sockets, dynamic libraries, random device, etc.) in RAII-safe C++ types. It builds as a shared library (`liboscpp`) via CMake and is installed as a normal system library with a CMake package config for downstream `find_package(oscpp)` consumers.

| Component         | Purpose                                                                 |
|--------------------|--------------------------------------------------------------------------|
| `circular_buffer`  | Thread-safe fixed-capacity ring buffer with blocking and `try` put/get.  |
| `dynamiclibrary`   | Introspect/load a dynamically loaded library.                            |
| `file`             | POSIX file wrapper (open/close/mmap); throws `SysException` on error.    |
| `file_descriptor`  | Generic non-copyable, movable file/handle descriptor wrapper.            |
| `random_device`    | Wrapper around the system random device.                                 |
| `socket`           | Socket wrapper (non-copyable, movable).                                  |
| `stopwatch`        | Timing utility for code sections.                                        |
| `sysexception`     | `std::system_error` subclass built from `errno` (generic category).      |
| `trim`             | Whitespace trimming for strings.                                         |

## Build

Requires CMake >= 3.25 and a C++23 compiler. Tests require Boost (`unit_test_framework` component, dynamic linking).

Tests build by default only when oscpp is the top-level project; pass `-DOSCPP_BUILD_TESTING=OFF` to skip them (and the Boost requirement). Build out of tree, never inside the source directory. The build directory for Claude is `../build/oscpp/claude`.

```sh
cmake -S . -B ../build/oscpp/claude
cmake --build ../build/oscpp/claude
```

Install (default prefix is `$HOME` unless overridden):

```sh
cmake --install ../build/oscpp/claude
```

## Test

Tests use Boost.Test, registered with CTest. All unit tests build into a single executable, `oscpp_tests`
(driver in `test/oscpp_tests.cpp`). CTest discovers every test case in it after the build (`cmake/OscppBoostTestDiscovery.cmake`, which lists the executable's contents), so each case is its own CTest test named `<suite>/<case>` and `ctest -R` can select suites or single cases. New test cases need no CMake change.

```sh
ctest --test-dir ../build/oscpp/claude                                # run all tests
ctest --test-dir ../build/oscpp/claude -LE stress                     # unit tests only
ctest --test-dir ../build/oscpp/claude -R '^File/'                    # one suite
ctest --test-dir ../build/oscpp/claude -R 'File/Test_map$'            # one case
../build/oscpp/claude/test/oscpp_tests --run_test=File/Test_map       # or run the binary directly
```

Warnings: oscpp's own code builds with `-Wall -Wextra -Wpedantic -Wconversion -Wshadow` and must stay warning-free. Configure with `-DOSCPP_WERROR=ON` to make warnings errors.

Formatting: sources use 2-space indentation, enforced by `.clang-format` (and `.editorconfig`). Run `cmake --build ../build/oscpp/claude --target format` to reformat and `--target check-format` to verify; both need `clang-format` (`pip install clang-format` or `pipx install clang-format` work where Homebrew has no binary, for example on Intel macOS). Every file starts with the same two lines: the Emacs modeline `// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-` and `// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.`

Presets: `CMakePresets.json` defines `debug`, `release`, `asan` (address and undefined sanitizers) and `tsan` (thread sanitizer) for configure, build and test, each building in `../build/oscpp/<preset>` with warnings as errors. For example `cmake --preset asan && cmake --build --preset asan && ctest --preset asan`. The extra test presets `debug-unit` (skips the stress tests) and `tsan-stress` (only the stress tests, under ThreadSanitizer) select subsets. Personal overrides go in `CMakeUserPresets.json`, which is git-ignored.

CI: `.github/workflows/ci.yml` runs on pushes to `main` and on pull requests. It builds and tests the `debug`, `release`, `asan` and `tsan` presets on macOS, installs oscpp and builds `test/package_consumer` against the installed package with `find_package(oscpp)`, and checks formatting on Linux with `clang-format==23.1.1` (pinned, because formatting differs between versions). Keep those steps runnable locally with the same commands.

Changelog: record user-visible changes in `CHANGELOG.md` under `[Unreleased]` (Keep a Changelog format), and breaking changes under a "Breaking changes" heading, since the project follows Semantic Versioning. The version lives in `project(oscpp VERSION ...)` in `CMakeLists.txt`.

Sanitizers without presets: configure a separate build directory with `-DOSCPP_SANITIZE=address\;undefined` or `-DOSCPP_SANITIZE=thread` (for example `../build/oscpp/claude-asan` and `../build/oscpp/claude-tsan`). `thread` can't be combined with `address`. Run the stress tests under `thread`.

Multithreaded stress tests are not unit tests. They live in their own executable (`test/stress_circular_buffer.cpp` -> `stress_circular_buffer`) and carry the CTest label `stress`. Run them alone with `ctest --test-dir ../build/oscpp/claude -L stress`.

A new unit test file goes in the `oscpp_tests` source list in `test/CMakeLists.txt` (re-run the build so discovery sees new cases). A new stress test needs its own `add_executable`/`target_link_libraries`/`add_test` block there, with the `stress` label.

## Architecture Conventions

- The throwing API lives in the `oscpp` namespace, one header/source pair per component under `include/oscpp/` and `src/oscpp/`. Include as `#include "oscpp/<component>.hpp"`. `circular_buffer` is a header-only template with no source file.
- Public classes/functions are annotated `OSCPP_API` (from `include/oscpp_export.hpp`, shared by both namespaces) since the library is built with hidden visibility; anything meant to be usable outside the shared library must carry it. The macro expands to `__attribute__((visibility("default")))` on GCC/Clang, and to `dllexport`/`dllimport` on Windows (untested).
- Resource-owning types (`FileDescriptor`, `Socket`, `File`) follow a consistent RAII pattern: copy constructor/assignment deleted, move constructor and move assignment provided (assignment closes the old resource first), destructor releases the resource, `valid()`/`explicit operator bool` and `release()` are available, and a `clone()`/`create()` static factory is used instead of a public copy path.
- Error handling in `oscpp` is via `oscpp::SysException` (in `sysexception.hpp`), a `std::system_error` constructed from `errno` or a `std::error_code`, rather than return codes.
- Every `oscpp` module may have a non-throwing counterpart in the `oscpp_exceptionless` namespace, which is a co-equal of `oscpp`, not nested in it. Code using one should not use the other. Headers and sources live in `include/oscpp_exceptionless/<component>.hpp` and `src/oscpp_exceptionless/<component>.cpp` (include as `"oscpp_exceptionless/<component>.hpp"`), with no aggregate header. Factories return `std::expected<T, std::error_code>` (errno in the generic category); `DynamicLibrary` uses `std::expected<T, DynamicLibraryError>`, a fixed-size buffer holding the `dlerror()` text. Nothing in `oscpp_exceptionless` may throw: every function is `noexcept`, the public API uses no allocating types such as `std::string` (they can throw `std::bad_alloc` when built or copied), any STL call that can throw (`std::make_unique`, `std::random_device`) is wrapped in `try`/`catch` and turned into an error value, and each counterpart's test file has `static_assert`s that check `noexcept`. `oscpp` classes wrap their `oscpp_exceptionless` counterpart and translate errors into `SysException`. Currently: `DynamicLibrary`, `File`, `FileDescriptor`, `RandomDevice`, `Socket`.
- Test files follow the pattern `test/test_<component>.cpp` (`test/test_exceptionless_<component>.cpp` for the `oscpp_exceptionless` counterpart), using `BOOST_TEST_DYN_LINK` + `BOOST_AUTO_TEST_CASE`, and include the header under test by its directory form (e.g. `#include "oscpp/trim.hpp"`) (the tests link `oscpp`, which provides the include path).
- Tests that need files must use `TempDirectory` (`test/temp_directory.hpp`) and never write to the working directory.
