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
  - `oscpp_exceptionless::DynamicLibrary` reports errors as a `DynamicLibraryError`: a fixed 256-byte, trivially
    copyable buffer holding the `dlerror()` text (truncated if longer). It is not a `std::string` because building or
    copying one can throw `std::bad_alloc`, and there is no errno to put in an `error_code`. `RandomDevice::create`
    takes a `const char *` token for the same reason. Consider a custom `std::error_category` if callers need to
    branch on the failure.
  - `oscpp_exceptionless::RandomDevice` is not a `std::uniform_random_bit_generator`, because a draw can fail.
    libc++ silently accepts unrecognized tokens, so the token failure path is untested.
- [x] **`Socket` and `FileDescriptor` duplicated each other.** `oscpp_exceptionless::Socket` now owns a
  `oscpp_exceptionless::FileDescriptor` (composition, not inheritance: `clone()` must return a `Socket`, and
  inheritance invites slicing) and exposes it through `fileDescriptor()`. `oscpp::Socket` wraps the exceptionless
  socket. Still missing: socket operations (bind, connect, send, recv) and `read`/`write` on `FileDescriptor`.
- [x] `DynamicLibrary` takes a `dlopen` flags parameter (default `RTLD_NOW | RTLD_LOCAL`) in both namespaces, instead of the literal `0`.
- [x] Missing members added in both namespaces:
  - move assignment for `File`, `FileDescriptor`, `Socket` and `DynamicLibrary` (close-then-take, safe for
    self-assignment); `DynamicLibrary` already had a move constructor
  - `valid()`, `explicit operator bool` and `release()` on `File`, `FileDescriptor` and `Socket`
  - `read`/`write` on `FileDescriptor` (POSIX semantics: short counts, 0 at end of file)
  - a public `descriptor()` and `unmap()` on `File`
  Not added: `valid()`/`release()` on `DynamicLibrary`, and `const` versions of `read`/`write`.
- [x] **`[[maybe_unused]]` was applied to public classes and methods** (`DynamicLibrary`, `FileDescriptor`, `Socket`,
  `trim`). It's meaningless there and hides real dead-code warnings. Removed.
- [x] `OSCPP_API` macro in `include/oscpp_export.hpp` replaces the repeated `__attribute__((visibility("default")))`. On Windows it
  expands to `dllexport`/`dllimport` (`OSCPP_EXPORTS` is defined when building the library), which is untested.
- [x] `trim`: the docs and a comment say it trims NULs and whitespace from the right end, `isspace` now gets an
  `unsigned char` (it is undefined for negative `char` values; covered by a test), and `<cctype>` is not needed. A
  `std::string_view` overload was considered and not added, since `trim` modifies its argument in place.
- [x] `CircularBuffer` cleanup: it is now header-only (`src/oscpp/circular_buffer.cpp` and its explicit instantiations are
  gone, so any `ElementType` works without a closed instantiation set), the leaked `using std::unique_lock; using std::mutex;`
  declarations are removed, `<algorithm>`, `<cstddef>`, `<mutex>` and `<condition_variable>` are included, `size_t` is
  `std::size_t`, and the docs describe the heap-allocated storage. The stress tests still pass under TSan.
- [ ] Deferred (nothing uses `CircularBuffer` yet): timeout variants (`try_*_for`) and move support. A default-constructible
  `ElementType` is an accepted requirement.
- [x] `file.hpp` and `socket.hpp` include `<utility>` and `<cstddef>` where used. `<fcntl.h>` and `<sys/stat.h>` stay in `file.hpp`
  because the default arguments and `struct stat` need them.

## P2: Build system (`CMakeLists.txt`, `test/CMakeLists.txt`)

- [x] `OSCPP_BUILD_TESTING` option (default ON only when top-level), so `FetchContent` consumers don't need Boost.
- [x] Install directories come from `GNUInstallDirs` (`CMAKE_INSTALL_*DIR`) instead of hard-coded `bin`/`lib`/`include`.
- [x] Install prefix defaults to `$HOME` so test installs need no privileged account. This is intentional: it applies
  only when no prefix was given, and `--prefix` or `-DCMAKE_INSTALL_PREFIX=` always wins (documented in `CMakeLists.txt`
  and `CLAUDE.md`). Not a change to make.
- [x] Visibility preset hidden, `VISIBILITY_INLINES_HIDDEN`, `CMAKE_CXX_STANDARD_REQUIRED ON`, `CMAKE_CXX_EXTENSIONS OFF`, and
  `target_compile_features(oscpp PUBLIC cxx_std_23)` so consumers inherit the requirement.
- [x] `PUBLIC_HEADER` flattened the install layout. Headers now live in `include/oscpp/` and
  `include/oscpp_exceptionless/` and install to the same directories (`install(DIRECTORY ...)`).
- [x] The export file is `oscpp-targets.cmake`, and `oscpp::oscpp` is an `ALIAS` so in-tree and installed usage match. Verified
  with an install plus a `find_package(oscpp)` consumer project.
- [x] Removed the nested `project(oscpp_test)` and the redundant `../include` directory from `test/CMakeLists.txt`.
- [x] Tests are registered by discovery (`cmake/OscppBoostTestDiscovery.cmake`): each Boost.Test case is its own CTest test named
  `<suite>/<case>`, so `ctest -R` selects suites and single cases, and new cases need no CMake change.
- [x] Sanitizer option: `-DOSCPP_SANITIZE=address\;undefined` or `-DOSCPP_SANITIZE=thread` (see `CMakeLists.txt`).
- [x] Warnings interface target `oscpp_warnings` (`-Wall -Wextra -Wpedantic -Wconversion -Wshadow`) is applied to the
  library and tests, but not propagated to consumers. `-DOSCPP_WERROR=ON` makes warnings errors. The build is currently
  warning-free, including the sanitizer build. Boost headers are passed as `-isystem` so only oscpp's code is checked.
- [x] `CMakePresets.json` with `debug`, `release`, `asan` and `tsan` configure, build and test presets (binary dirs in
  `../build/oscpp/<preset>`, warnings as errors), plus `debug-unit` and `tsan-stress` test presets.
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

- [x] README rewritten: typos fixed, both namespaces explained, an example, and build, install, options and
  `find_package(oscpp)` usage.
- [x] `.gitignore` stale Visual Studio entries replaced (now `cmake-build-*`, `build/` and `CMakeUserPresets.json`).
- [x] Normalized headers and sources: every file starts with the same Emacs modeline and a `Copyright 2026` line, `#endif //
  GUARD` comments use the real guard names (the exceptionless guards are `OSCPP_EXCEPTIONLESS_*`), files end with a newline,
  and everything is formatted with `clang-format` (2-space indent, 120 columns, `.clang-format`). The `format` and
  `check-format` CMake targets reformat and verify. clang-format isn't available from Homebrew on Intel macOS, so install
  it with `pip install clang-format` or `pipx install clang-format`.
- [x] The `-*- mode:C++ ... -*-` modelines are kept (they carry the Emacs settings); `.editorconfig` covers other editors.
- [ ] Bump the version and add a CHANGELOG once the API changes above land, since the error-handling split (`create`/`clone`
  now throw, non-throwing forms moved to `oscpp_exceptionless`) is source-breaking.

## Deferred until a Linux environment exists

Not verified, so not actionable yet. Re-check when a Linux clang environment is available.

- [x] `dlopen(path, 0)`: glibc requires `RTLD_LAZY` or `RTLD_NOW`. Fixed by the flags parameter (default `RTLD_NOW | RTLD_LOCAL`).
- [ ] `/dev/random` blocking behavior on older kernels.
- [ ] Tests that hard-code macOS paths and error text.
- [x] `lib64` install layout from the hard-coded `LIBDIR`. Fixed by using `GNUInstallDirs`.
- [ ] Add an Ubuntu (clang) CI job; consider gcc support then.
