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

- [x] **Pick one error-handling model.** Decided: two co-equal namespaces. `oscpp` (`include/oscpp/`,
  `src/oscpp/`) throws `SysException`; `oscpp_exceptionless` (`include/oscpp_exceptionless/`,
  `src/oscpp_exceptionless/`) returns `std::expected<T, std::error_code>` (errno in the generic category). Callers use
  one or the other. `oscpp` types may wrap their `oscpp_exceptionless` counterpart and translate errors to exceptions.
  Done for `FileDescriptor` and `Socket`; the old `std::pair<T, int>` returns are gone.
- [ ] Finish the error-handling split:
  - [x] `oscpp_exceptionless` counterparts now exist for `File`, `DynamicLibrary` and `RandomDevice`, and the `oscpp`
    versions wrap them. Pure-computation modules (`trim`, `StopWatch`, `CircularBuffer`) need none.
  - [x] `SysException` now derives from `std::system_error` (callers can inspect `code()`), has an `error_code`
    constructor, and uses `std::generic_category().message()` instead of `strerror_r`. This also removes the
    `strerror_r` GNU/XSI portability item below.
  - `oscpp::DynamicLibrary` still throws plain `std::runtime_error`, because `dlerror()` text has no errno to put in
    a `std::error_code`. Decide between a custom `std::error_category` and leaving it.
  - `oscpp_exceptionless::DynamicLibrary` reports errors as the `dlerror()` string (`std::expected<T, std::string>`)
    since there is no errno to put in an `error_code`. Consider a custom `std::error_category` if callers need to
    branch on the failure.
  - `oscpp_exceptionless::RandomDevice` is not a `std::uniform_random_bit_generator`, because a draw can fail.
    libc++ silently accepts unrecognized tokens, so the token failure path is untested.
- [x] **`Socket` and `FileDescriptor` duplicated each other.** `oscpp_exceptionless::Socket` now owns a
  `oscpp_exceptionless::FileDescriptor` (composition, not inheritance: `clone()` must return a `Socket`, and
  inheritance invites slicing) and exposes it through `fileDescriptor()`. `oscpp::Socket` wraps the exceptionless
  socket. Still missing: socket operations (bind, connect, send, recv) and `read`/`write` on `FileDescriptor`.
- [x] `DynamicLibrary` takes a `dlopen` flags parameter (default `RTLD_NOW | RTLD_LOCAL`) in both namespaces, instead of the literal `0`.
- [ ] Add missing members:
  - move assignment for `File`, `FileDescriptor` and `Socket` (currently deleted or absent; implement with
    close-then-take or swap)
  - a move constructor for `DynamicLibrary`
  - `release()` and `explicit operator bool` / `valid()` on the handle types
  - `read`/`write` on `FileDescriptor`
  - a public `descriptor()` on `File`
  - a public `unmap()` on `File`
- [x] **`[[maybe_unused]]` was applied to public classes and methods** (`DynamicLibrary`, `FileDescriptor`, `Socket`,
  `trim`). It's meaningless there and hides real dead-code warnings. Removed.
- [ ] **Replace the repeated `__attribute__((visibility("default")))`** with an `OSCPP_API` macro in an
  `export.hpp`. Then `#include`s no longer depend on compiler-specific syntax, and MSVC becomes possible later.
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
- [x] `file.hpp` and `socket.hpp` include `<utility>` and `<cstddef>` where used. `<fcntl.h>` and `<sys/stat.h>` stay in `file.hpp`
  because the default arguments and `struct stat` need them.
- [ ] `SysException` messages carry no context. Add an optional `what_arg` (e.g. `"open(/path): No such file"`), and use
  it in `File`.

## P2: Build system (`CMakeLists.txt`, `test/CMakeLists.txt`)

- [x] `OSCPP_BUILD_TESTING` option (default ON only when top-level), so `FetchContent` consumers don't need Boost.
- [x] Install directories come from `GNUInstallDirs` (`CMAKE_INSTALL_*DIR`) instead of hard-coded `bin`/`lib`/`include`.
- [ ] Forcing `CMAKE_INSTALL_PREFIX` to `$HOME` is surprising for a library. Document it, or gate it behind an option
  (e.g. `OSCPP_DEV_PREFIX`).
- [x] Visibility preset hidden, `VISIBILITY_INLINES_HIDDEN`, `CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`, and
  `target_compile_features(oscpp PUBLIC cxx_std_23)` so consumers inherit the requirement.
- [x] `PUBLIC_HEADER` flattened the install layout. Headers now live in `include/oscpp/` and
  `include/oscpp_exceptionless/` and install to the same directories (`install(DIRECTORY ...)`).
- [x] The export file is `oscpp-targets.cmake`, and `oscpp::oscpp` is an `ALIAS` so in-tree and installed usage match. Verified
  with an install plus a `find_package(oscpp)` consumer project.
- [x] Removed the nested `project(oscpp_test)` and the redundant `../include` directory from `test/CMakeLists.txt`.
- [ ] Register tests via `boost_tests` discovery (or at least one `add_test` per suite) so `ctest -R` can select
  individual suites. CLAUDE.md currently describes the old one-executable-per-file layout.
- [x] Sanitizer option: `-DOSCPP_SANITIZE=address\;undefined` or `-DOSCPP_SANITIZE=thread` (see `CMakeLists.txt`).
- [x] Warnings interface target `oscpp_warnings` (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow`) is applied to the
  library and tests, but not propagated to consumers. `-DOSCPP_WERROR=ON` makes warnings errors. The build is currently
  warning-free, including the sanitizer build. Boost headers are passed as `-isystem` so only oscpp's code is checked.
- [ ] Add `CMakePresets.json` (debug, release, asan) with binary dirs outside the source tree.
- [x] Deleted `sampleDynamic.cpp` (never wired up) and `mock_strerror.cpp` (unused; `SysException::message` no longer
  calls `strerror_r`). The CLAUDE.md instruction to keep `mock_strerror.cpp` was removed at the same time.

## P3: Tests

- [x] Removed the bogus `#define BOOST_BOOST_AUTO_TEST_MODULE ...` from all test files.
- [x] Tests that write files now use the `TempDirectory` helper (`test/temp_directory.hpp`): a unique directory under the
  system temp directory, removed on destruction, with per-test fixtures. Nothing is written to the working directory.
- [x] Missing coverage is filled. The `CircularBuffer` stress tests (`stress_circular_buffer`, label `stress`) ran clean
  under TSan (three runs, AppleClang 21, `-fsanitize=thread` on the library and test, build dir
  `../build/oscpp/claude-tsan`). Making that repeatable is covered by the sanitizer option in the build-system section.
- [ ] `test_system_exception.cpp` expects the macOS text "Undefined error: 0" and `test_dynamic_library.cpp` hard-codes
  `/usr/lib/libc++.1.dylib`. Fine while macOS-only; revisit when Linux returns.
- [x] `test_random_device` no longer asserts `entropy() > 0`; it reports it, and the generator use with the standard
  distributions is covered by `test_random_device_as_generator`.
- [ ] Add CI (GitHub Actions on macOS with clang, ASan/UBSan job, TSan job for `CircularBuffer`).

## P4: Docs and hygiene

- [ ] README: fix the typos ("associed", "aruound" in `socket.hpp`, "oen" in `circular_buffer.hpp`), remove trailing
  whitespace in the table, and add build, install and `find_package(oscpp)` usage sections plus a minimal example.
- [ ] `.gitignore` carries stale Visual Studio entries (`OSCPP.vpwhistu`, `OSCPP.vtg`, `Debug`). Replace with
  `cmake-build-*` (already present) plus `CMakeUserPresets.json`. `build/` is already ignored.
- [ ] Normalize headers:
  - copyright years and formats vary (2016, 2021, 2023, "©2026")
  - indentation switches between 2 and 4 spaces
  - `#endif` comments are inconsistent (`stopwatch.hpp` has the wrong guard name in its comment; `sysexception.hpp` and
    `trim.hpp` have none)
  - some files lack a trailing newline
  Add a `.clang-format` (and optionally `.clang-tidy`) and run it once.
- [ ] Replace the `-*- mode:C++ ... -*-` Emacs modelines with a `.editorconfig`, if they aren't wanted.
- [ ] Bump the version and add a CHANGELOG once the API changes above land, since the error-handling split (`create`/`clone`
  now throw, non-throwing forms moved to `oscpp_exceptionless`) is source-breaking.

## Deferred until a Linux environment exists

Not verified, so not actionable yet. Re-check when a Linux clang environment is available.

- [x] `dlopen(path, 0)`: glibc requires `RTLD_LAZY` or `RTLD_NOW`. Fixed by the flags parameter (default `RTLD_NOW | RTLD_LOCAL`).
- [ ] `/dev/random` blocking behavior on older kernels.
- [ ] Tests that hard-code macOS paths and error text.
- [x] `lib64` install layout from the hard-coded `LIBDIR`. Fixed by using `GNUInstallDirs`.
- [ ] Add an Ubuntu (clang) CI job; consider gcc support then.
