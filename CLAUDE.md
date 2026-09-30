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

Build out of tree, never inside the source directory. The build directory for Claude is `../build/oscpp/claude`.

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
(driver in `test/oscpp_tests.cpp`), run by CTest as `boost_tests`. CTest therefore can't select individual suites; run
the binary directly for that.

```sh
ctest --test-dir ../build/oscpp/claude                                # run all tests
ctest --test-dir ../build/oscpp/claude -LE stress                     # unit tests only
../build/oscpp/claude/test/oscpp_tests --run_test=File                # one suite (or File/Test_map for one case)
```

Multithreaded stress tests are not unit tests. They live in their own executable (`test/stress_circular_buffer.cpp` -> `stress_circular_buffer`) and carry the CTest label `stress`. Run them alone with `ctest --test-dir ../build/oscpp/claude -L stress`.

A new unit test file goes in the `oscpp_tests` source list in `test/CMakeLists.txt`. A new stress test needs its own `add_executable`/`target_link_libraries`/`add_test` block there, with the `stress` label.

## Architecture Conventions

- The throwing API lives in the `oscpp` namespace, one header/source pair per component under `include/oscpp/` and `src/oscpp/`. Include as `#include "oscpp/<component>.hpp"`.
- Public classes/functions are annotated `__attribute__((visibility("default")))` since the library is built with `-fvisibility=hidden`; anything meant to be usable outside the shared library must carry this attribute.
- Resource-owning types (`FileDescriptor`, `Socket`, `File`) follow a consistent RAII pattern: copy constructor/assignment deleted, move constructor provided, destructor releases the resource, and a `clone()`/`create()` static factory is used instead of a public copy path.
- Error handling in `oscpp` is via `oscpp::SysException` (in `sysexception.hpp`), a `std::system_error` constructed from `errno` or a `std::error_code`, rather than return codes.
- Every `oscpp` module may have a non-throwing counterpart in the `oscpp_exceptionless` namespace, which is a co-equal of `oscpp`, not nested in it. Code using one should not use the other. Headers and sources live in `include/oscpp_exceptionless/<component>.hpp` and `src/oscpp_exceptionless/<component>.cpp` (include as `"oscpp_exceptionless/<component>.hpp"`), with no aggregate header. Factories return `std::expected<T, std::error_code>` (errno in the generic category); `DynamicLibrary` uses `std::expected<T, std::string>` holding the `dlerror()` text. `oscpp` classes wrap their `oscpp_exceptionless` counterpart and translate errors into `SysException`. Currently: `DynamicLibrary`, `File`, `FileDescriptor`, `RandomDevice`, `Socket`.
- Test files follow the pattern `test/test_<component>.cpp` (`test/test_exceptionless_<component>.cpp` for the `oscpp_exceptionless` counterpart), using `BOOST_TEST_DYN_LINK` + `BOOST_AUTO_TEST_CASE`, and include the header under test by its directory form (e.g. `#include "oscpp/trim.hpp"`) via the `../include` include path set in `test/CMakeLists.txt`.
- Tests that need files must use `TempDirectory` (`test/temp_directory.hpp`) and never write to the working directory.

test/mock_strerror.cpp exists an an example of dynamic symbol interjection. It is currently unused, but preserve it.