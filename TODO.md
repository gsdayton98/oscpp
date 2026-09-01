# oscpp TODO

Tracking issues found in a code review (2026-08-31). Ordered by severity.

## Critical

- [ ] Make the build reproducible: `test/CMakeLists.txt` includes `$ENV{HOME}/Projects` so `test_circular_buffer` only compiles because `~/Projects/CppUnitXLite` happens to exist on this machine. Fails on any clean clone/CI.
- [ ] Migrate `test/test_circular_buffer.cpp` off CppUnitXLite to Boost.Test, matching every other test file (missed by the `0875897` migration commit).
- [x] Fix `CircularBuffer` thread-safety: `empty()`/`full()` are read without holding a lock, and `tryGet`/`tryPut` check them before acquiring `readMutex`/`writeMutex` (check-then-act race). `volatile` on `head`/`tail` does not provide atomicity or cross-thread visibility. Fixed by consolidating to a single `guard` mutex protecting `head`/`tail`; verified clean under ThreadSanitizer with a 4-producer/4-consumer stress test (the old code reliably raced and corrupted data under the same test).
- [x] Decide how to handle degenerate zero-usable-capacity buffers (`CircularBuffer(0)`, and any construction where `capacity()==1`): `put()`/`get()` deadlock forever. At minimum document it; consider rejecting it in the constructor.

## High

- [x] Fix `unsigned int k` vs `size_t n` mismatch in `CircularBuffer::roundup` (overflow risk for very large requested sizes).
- [X] Document (or eliminate) the fact that `CircularBuffer<T>`'s implementation lives in `src/circular_buffer.cpp` and is explicitly instantiated for a fixed list of built-in types only — other `T` fail at link time with no explanation from the header.
- [X] Make `DynamicLibrary` throw `oscpp::SysException` (or an equivalent) instead of bare `std::runtime_error(dlerror())`, for consistency with the rest of the library; guard against `dlerror()` returning `nullptr`.
- [x] Fix `File::map()` to release any existing mapping before creating a new one (currently leaks the old `mmap` region if called twice).

## Medium

- [x] Remove the pessimizing `std::move()` around prvalue returns in `FileDescriptor::create()` and `FileDescriptor::clone()` (`src/file_descriptor.cpp`).
- [x] Remove the duplicate `src/circular_buffer.cpp` entry in `CMakeLists.txt` `SOURCES`.
- [x] Remove or wire up `test/mock_strerror.cpp` (currently unreferenced in `test/CMakeLists.txt`).

## Low / hygiene

- [x] Remove the leftover "Project Euler has confidentiality rules, so do not copy nor publish this code" header from `include/file.hpp` / `src/file.cpp` — contradicts the repo's actual `LICENSE`.
- [x] Fix the stale header guard `TEST_RANDOM_DEVICE_RANDOM_DEVICE_HPP` in `include/random_device.hpp`.
- [x] Standardize header guard naming (`OSCPP_*` prefix) across all headers.
- [x] Fix filename typo: `src/dynamiclibary.cpp` -> `src/dynamiclibrary.cpp`.
