# oscpp TODO

Findings from a review of `ai_refactor` (macOS/AppleClang 21; clang is the only supported compiler for now). The default build and tests pass, and a
`-Wall -Wextra -Wpedantic -Wconversion -Wshadow -fsanitize=address,undefined` build also passes its tests.
Items are ordered by priority within each section. Nothing here has been fixed yet.

## P0: Correctness bugs

- [x] **`RandomDevice` doesn't model `UniformRandomBitGenerator`** (`include/random_device.hpp`). It has no
  `result_type`, so it can't be passed to `std::shuffle` or `std::uniform_int_distribution`. Add `using result_type = unsigned int;`.
  It also hard-codes `/dev/random`; let the caller pick the token, defaulting to `std::random_device{}`.
- [x] `File::map()` on an empty file fails, because `mmap` with length 0 gives `EINVAL`. Either handle that case
  (return `{nullptr, 0}`) or document it. Also fix the `off_t` to `size_t` sign conversion (`src/file.cpp:21`).
- [x] `dup()` and `socket()` don't set close-on-exec. Use `fcntl(F_DUPFD_CLOEXEC)` in `FileDescriptor::clone` and
  `Socket::clone`, and set `FD_CLOEXEC` on new sockets.
- [x] `CircularBuffer` dtor definition triggers `-Wdtor-name` (`include/circular_buffer.hpp:124`). Two other
  problems there:
  - The out-of-class definitions are indented as if they were inside the namespace, and `tryGet`'s closing brace is
    misaligned. This looks like a botched merge.
  - `size()` relies on operator precedence (`a + b - c & mask`). Add parentheses.

## P1: API and design consistency

- [ ] **Pick one error-handling model.** CLAUDE.md says "errors via `SysException`", but `FileDescriptor::clone`,
  `Socket::create` and `Socket::clone` return `std::pair<T, int>` with errno, and `DynamicLibrary` throws
  `std::runtime_error`. Options:
  - `std::expected<T, std::error_code>` (we're on C++23), or
  - throwing everywhere.
  Also make `SysException` derive from `std::system_error` so callers can inspect `code()`. That fixes
  `DynamicLibrary` being the odd one out. `std::generic_category().message(errno)` would also replace the
  `strerror_r` buffer code in `SysException::message`.
- [ ] **`Socket` and `FileDescriptor` duplicate each other.** Socket is a handle plus `descriptor()`/`clone()`, with no
  bind, connect, send or recv. Have it own a `FileDescriptor` (or share a base) and add the missing operations,
  or document it as a bare handle.
- [ ] `DynamicLibrary`: take a `dlopen` flags parameter (default `RTLD_NOW | RTLD_LOCAL`) instead of the literal `0`.
- [ ] Add missing members:
  - move assignment for `File`, `FileDescriptor` and `Socket` (currently deleted or absent; implement with
    close-then-take or swap)
  - a move constructor for `DynamicLibrary`
  - `release()` and `explicit operator bool` / `valid()` on the handle types
  - `read`/`write` on `FileDescriptor`
  - a public `descriptor()` on `File`
  - a public `unmap()` on `File`
- [ ] **`[[maybe_unused]]` is applied to public classes and methods** (`DynamicLibrary`, `FileDescriptor`, `Socket`,
  `trim`). It's meaningless there and hides real dead-code warnings. Remove it.
- [ ] **Replace the repeated `__attribute__((visibility("default")))`** with an `OSCPP_API` macro in an
  `export.hpp`. Then `#include`s no longer depend on compiler-specific syntax, and MSVC becomes possible later.
- [ ] `File::open` is a private static that shadows `::open`, and `File::close` shadows `::close`. Rename them
  (e.g. `openOrThrow`) so calls read unambiguously.
- [ ] `trim`:
  - The name suggests both ends, but it trims only trailing whitespace and NULs, and the NUL part is undocumented.
    Rename it to `rtrim`, or add `ltrim`/`trim` and document the NUL behavior.
  - `isspace(char)` is UB for negative values, so cast to `unsigned char`.
  - `<cctype>` isn't included.
  - Consider a `std::string_view` overload.
- [ ] `CircularBuffer`:
  - The docs say "static array", but the storage is heap-allocated.
  - Elements must be default-constructible, since `new T[]` is used.
  - There's no move support and no `try_*`/timeout variants.
  - `using std::unique_lock; using std::mutex;` at global scope in a public header leaks names, so remove them.
  - Implementations in the header plus explicit instantiations in `.cpp`, with no `extern template`, is confusing.
    Decide whether it's header-only (then drop the `.cpp`) or has a closed instantiation set (then add
    `extern template`).
  - Add `<cstddef>` and `<algorithm>` includes.
- [ ] Add missing standard includes (`<utility>`, `<cstddef>`) to `file.hpp` and `socket.hpp`. Also stop leaking
  `<fcntl.h>` and `<sys/stat.h>` from `file.hpp` unless needed for the default arguments.
- [ ] `SysException` messages carry no context. Add an optional `what_arg` (e.g. `"open(/path): No such file"`), and use
  it in `File`.

## P2: Build system (`CMakeLists.txt`, `test/CMakeLists.txt`)

- [ ] Add an `OSCPP_BUILD_TESTING` option (default ON only when top-level). Currently `add_subdirectory(test)` is
  unconditional, so anyone consuming oscpp via `FetchContent` needs Boost.
- [ ] Don't hard-override `BINDIR/LIBDIR/INCLUDEDIR`. Use the `CMAKE_INSTALL_*DIR` variables from `GNUInstallDirs`,
  which are already included. The current values ignore the platform's conventions.
- [ ] Forcing `CMAKE_INSTALL_PREFIX` to `$HOME` is surprising for a library. Document it, or gate it behind an option
  (e.g. `OSCPP_DEV_PREFIX`).
- [ ] Use `CXX_VISIBILITY_PRESET hidden` / `VISIBILITY_INLINES_HIDDEN` and `CMAKE_CXX_STANDARD_REQUIRED ON` /
  `CMAKE_CXX_EXTENSIONS OFF`. Better still, use `target_compile_features(oscpp PUBLIC cxx_std_23)` so consumers inherit
  the requirement.
- [ ] `PUBLIC_HEADER` flattens the install layout. Install headers to `include/oscpp/` and use
  `#include <oscpp/file.hpp>`, to avoid collisions on generic names like `file.hpp`, `socket.hpp` and `trim.hpp`.
- [ ] Rename the export file: `install(EXPORT oscpp ... )` writes `oscpp.cmake`, which sits beside `oscpp-config.cmake`
  and is easy to confuse. Use `oscpp-targets.cmake`. Also add an `ALIAS oscpp::oscpp` so in-tree and installed usage
  match.
- [ ] `test/CMakeLists.txt` starts a nested `project(oscpp_test)` before `cmake_minimum_required`. Remove it, and
  drop the redundant `../include` include directory, since the target already links the PUBLIC include dir.
- [ ] Register tests via `boost_tests` discovery (or at least one `add_test` per suite) so `ctest -R` can select
  individual suites. CLAUDE.md currently describes the old one-executable-per-file layout.
- [ ] Add a warnings interface target (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow`) and a sanitizer option. The
  ASan/UBSan run above was clean, so this is cheap to keep. Fix the remaining sign-compare warnings in
  `test_circular_buffer.cpp:144,345` and `test_system_exception.cpp:19`.
- [ ] Add `CMakePresets.json` (debug, release, asan) with binary dirs outside the source tree.
- [ ] `sampleDynamic.cpp` isn't referenced by any CMake target. Wire it up (used by the dynamic-library test) or
  delete it. `mock_strerror.cpp` is unused but must be **kept**, per CLAUDE.md.

## P3: Tests

- [ ] Remove the bogus `#define BOOST_BOOST_AUTO_TEST_MODULE ...` from all test files. It's a typo'd macro and does
  nothing. The module is defined in `oscpp_tests.cpp`.
- [ ] `test_file.cpp` writes `testFile.dat` (and `test_file_descriptor.cpp` writes `testFile.txt`) into the cwd and never
  removes them. Use a temp dir (`std::filesystem::temp_directory_path()` plus unique name) and clean up in the fixture
  destructor. `BOOST_TEST_GLOBAL_FIXTURE` inside a suite is also misleading, so use a per-suite fixture.
- [ ] Missing coverage:
  - `File`: open failure throws `SysException`, move construction, re-`map()`, write flags.
  - `FileDescriptor`: move semantics, `create(-1)`.
  - `Socket`: `create` failure path, move.
  - `DynamicLibrary`: missing library and missing symbol throw.
  - `StopWatch`: `reset`.
  - `trim`: empty string and all-whitespace edge cases.
  - `CircularBuffer`: multi-thread producer/consumer under TSan.
- [ ] `test_system_exception.cpp` expects the macOS text "Undefined error: 0" and `test_dynamic_library.cpp` hard-codes
  `/usr/lib/libc++.1.dylib`. Fine while macOS-only; revisit when Linux returns.
- [ ] `test_random_device` only asserts `entropy() > 0`, which is not guaranteed by the standard. Assert that it
  compiles and works as a generator (e.g. with `std::uniform_int_distribution`) instead.
- [ ] Add CI (GitHub Actions on macOS with clang, ASan/UBSan job, TSan job for `CircularBuffer`).

## P4: Docs and hygiene

- [ ] Update CLAUDE.md. The Test section describes one executable per source file, but there is now a single
  `oscpp_tests`. The "filenames drop letters" note is obsolete (`dynamiclibary.cpp` was fixed). It says `Stopwatch` but the
  class is `StopWatch`.
- [ ] README: fix the typos ("associed", "aruound" in `socket.hpp`, "oen" in `circular_buffer.hpp`), remove trailing
  whitespace in the table, and add build, install and `find_package(oscpp)` usage sections plus a minimal example.
- [ ] `.gitignore` carries stale Visual Studio entries (`OSCPP.vpwhistu`, `OSCPP.vtg`, `Debug`). Replace with
  `build/` and `cmake-build-*` (already present) plus `CMakeUserPresets.json`.
- [ ] Normalize headers:
  - copyright years and formats vary (2016, 2021, 2023, "©2026")
  - indentation switches between 2 and 4 spaces
  - `#endif` comments are inconsistent (`stopwatch.hpp` has the wrong guard name in its comment; `sysexception.hpp` and
    `trim.hpp` have none)
  - some files lack a trailing newline
  Add a `.clang-format` (and optionally `.clang-tidy`) and run it once.
- [ ] Replace the `-*- mode:C++ ... -*-` Emacs modelines with a `.editorconfig`, if they aren't wanted.
- [ ] Bump the version and add a CHANGELOG once the API changes above land, since the `Result`/exception change is
  source-breaking.

## Deferred until a Linux environment exists

Not verified, so not actionable yet. Re-check when a Linux clang environment is available.

- [ ] `SysException::message` uses the XSI `strerror_r` (returns `int`). glibc with `_GNU_SOURCE` exposes the GNU
  variant (returns `char*`), which would break the `switch`. Clang on Linux may hit this.
- [ ] `dlopen(path, 0)`: glibc requires `RTLD_LAZY` or `RTLD_NOW` (covered by the flags item above).
- [ ] `/dev/random` blocking behavior on older kernels.
- [ ] Tests that hard-code macOS paths and error text.
- [ ] `lib64` install layout from the hard-coded `LIBDIR`.
- [ ] Add an Ubuntu (clang) CI job; consider gcc support then.
