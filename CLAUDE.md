# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

oscpp is a C++23 header/source library that wraps POSIX/OS-specific facilities (file descriptors, files, sockets, dynamic libraries, random device, etc.) in RAII-safe C++ types. It builds as a shared library (`liboscpp`) via CMake and is installed as a normal system library with a CMake package config for downstream `find_package(oscpp)` consumers.

| Component         | Purpose                                                                 |
|--------------------|--------------------------------------------------------------------------|
| `circular_buffer`  | Double-ended queue backed by a static array with wrap-around.            |
| `dynamiclibrary`   | Introspect/load a dynamically loaded library.                            |
| `file`             | POSIX file wrapper (open/close/mmap); throws `SysException` on error.    |
| `file_descriptor`  | Generic non-copyable, movable file/handle descriptor wrapper.            |
| `random_device`    | Wrapper around the system random device.                                 |
| `socket`           | Socket wrapper (non-copyable, movable).                                  |
| `stopwatch`        | Timing utility for code sections.                                        |
| `sysexception`     | `std::runtime_error` subclass that renders a message from `errno`.       |
| `trim`             | Whitespace trimming for strings.                                         |

## Build

Requires CMake >= 3.25 and a C++23 compiler. Tests require Boost (`unit_test_framework` component, dynamic linking).

```sh
cmake -S . -B build
cmake --build build
```

Install (default prefix is `$HOME` unless overridden):

```sh
cmake --install build
```

## Test

Tests use Boost.Test (one executable per source file under `test/`), registered with CTest.

```sh
ctest --test-dir build              # run all tests
ctest --test-dir build -R test_file # run a single test by name
build/test/test_file                # or run a test binary directly
```

Test executable names mirror their source file (e.g. `test/test_socket.cpp` -> `test_socket`), configured individually in `test/CMakeLists.txt` — a new test file needs a matching `add_executable`/`target_link_libraries`/`add_test` block added there.

## Architecture Conventions

- Everything lives in the `oscpp` namespace, one header/source pair per component under `include/` and `src/` (note: some source filenames drop letters from the header name, e.g. `dynamiclibrary.hpp` -> `src/dynamiclibary.cpp`).
- Public classes/functions are annotated `__attribute__((visibility("default")))` since the library is built with `-fvisibility=hidden`; anything meant to be usable outside the shared library must carry this attribute.
- Resource-owning types (`FileDescriptor`, `Socket`, `File`) follow a consistent RAII pattern: copy constructor/assignment deleted, move constructor provided, destructor releases the resource, and a `clone()`/`create()` static factory is used instead of a public copy path.
- Error handling is via `oscpp::SysException` (in `sysexception.hpp`), constructed from `errno`, rather than return codes.
- Test files follow the pattern `test/test_<component>.cpp`, using `BOOST_TEST_DYN_LINK` + `BOOST_AUTO_TEST_CASE`, and include the header under test directly by name (e.g. `#include "trim.hpp"`) via the `../include` include path set in `test/CMakeLists.txt`.

test/mock_strerror.cpp exists an an example of dynamic symbol interjection. It is currently unused, but preserve it.