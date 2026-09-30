# oscpp

C++23 wrappers around operating system facilities (file descriptors, files, sockets, dynamic libraries, the random
device, and a few utilities) that make them safe to use: resources are released automatically, and handles can be moved
but never copied by accident.

There are two co-equal versions of the API. Use one or the other:

| Namespace              | Headers                  | Errors                                                          |
|------------------------|--------------------------|-----------------------------------------------------------------|
| `oscpp`                | `oscpp/<name>.hpp`       | Throws `oscpp::SysException` (a `std::system_error`).           |
| `oscpp_exceptionless`  | `oscpp_exceptionless/<name>.hpp` | Returns `std::expected<T, std::error_code>`. Nothing throws. |

The `oscpp` classes wrap their `oscpp_exceptionless` counterparts and turn errors into exceptions.

## Components

| Component          | Namespaces                    | Purpose                                                        |
|--------------------|-------------------------------|----------------------------------------------------------------|
| `circular_buffer`  | `oscpp`                       | Thread-safe fixed-capacity ring buffer (header-only template). |
| `dynamiclibrary`   | `oscpp`, `oscpp_exceptionless`| Load a dynamic library and look up symbols.                    |
| `file`             | `oscpp`, `oscpp_exceptionless`| Open, stat and memory-map a file.                              |
| `file_descriptor`  | `oscpp`, `oscpp_exceptionless`| Owning file descriptor with `clone`, `read` and `write`.       |
| `random_device`    | `oscpp`, `oscpp_exceptionless`| System random device (the `oscpp` one works with `<random>`).  |
| `socket`           | `oscpp`, `oscpp_exceptionless`| Owning socket handle.                                          |
| `stopwatch`        | `oscpp`                       | Timing utility for sections of code.                           |
| `sysexception`     | `oscpp`                       | `std::system_error` built from `errno`.                        |
| `trim`             | `oscpp`                       | Trim trailing whitespace and NULs from a string.               |

## Example

```cpp
#include <iostream>
#include "oscpp/file.hpp"
#include "oscpp_exceptionless/file.hpp"

int main() {
  // Throwing version.
  try {
    oscpp::File file("data.bin");
    auto [data, length] = file.map();
    std::cout << "mapped " << length << " bytes\n";
  } catch (const oscpp::SysException &ex) {
    std::cerr << "error: " << ex.what() << " (" << ex.code() << ")\n";
  }

  // Non-throwing version.
  auto file = oscpp_exceptionless::File::create("data.bin");
  if (!file) {
    std::cerr << "error: " << file.error().message() << "\n";
    return 1;
  }
  return 0;
}
```

## Building

Requires CMake 3.25 or later and a C++23 compiler (developed with AppleClang on macOS). The tests require Boost
(`unit_test_framework`). Build out of tree:

```sh
cmake -S . -B ../build/oscpp
cmake --build ../build/oscpp
ctest --test-dir ../build/oscpp
```

| Option                  | Effect                                                                                    |
|-------------------------|-------------------------------------------------------------------------------------------|
| `OSCPP_BUILD_TESTING`   | Build the tests. On by default only when oscpp is the top-level project.                  |
| `OSCPP_SANITIZE`        | Sanitizers, for example `-DOSCPP_SANITIZE=address\;undefined` or `-DOSCPP_SANITIZE=thread`. |
| `OSCPP_WERROR`          | Treat warnings in oscpp's own code as errors.                                             |

CMake presets cover the common configurations (`debug`, `release`, `asan`, `tsan`), each building in
`../build/oscpp/<preset>` with warnings as errors:

```sh
cmake --preset asan
cmake --build --preset asan
ctest --preset asan
```

`ctest -R` selects tests by name: every Boost.Test case is its own CTest test named `<suite>/<case>`, for example
`ctest -R ^File/` or `ctest -R File/Test_map`. The `debug-unit` test preset skips the stress tests, and `tsan-stress` runs only them under ThreadSanitizer.

The multithreaded stress tests carry the CTest label `stress`: `ctest -L stress` runs only them, `ctest -LE stress`
skips them.

## Continuous integration

GitHub Actions builds and tests the `debug`, `release`, `asan` and `tsan` presets on macOS, checks that the installed
package works from another CMake project (`test/package_consumer`), and checks formatting with `clang-format`.

## Installing

The install prefix defaults to `$HOME` when you don't choose one, so a test install needs no privileges. Pass
`--prefix` or `-DCMAKE_INSTALL_PREFIX=` to choose another.

```sh
cmake --install ../build/oscpp
```

## Using oscpp from CMake

```cmake
find_package(oscpp REQUIRED)
target_link_libraries(myprogram PRIVATE oscpp::oscpp)
```

If oscpp is installed somewhere CMake doesn't look, add its prefix to `CMAKE_PREFIX_PATH`. The target carries the C++23
requirement and the include directory, so `#include "oscpp/file.hpp"` works without further setup. To use oscpp from
another CMake project without installing it, `FetchContent` or `add_subdirectory` also provide `oscpp::oscpp`; the tests
aren't built in that case.

## License

See [LICENSE](LICENSE).
