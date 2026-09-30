// -*- mode: c++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Non-throwing counterpart of oscpp::File. Wrapper around POSIX file operations that protects against inadvertent
// copying of the file descriptor and guarantees release of resources. Failures are returned as a std::error_code
// (errno in the generic category) inside a std::expected.

#ifndef OSCPP_EXCEPTIONLESS_FILE_HPP
#define OSCPP_EXCEPTIONLESS_FILE_HPP
#include <cstddef>
#include <expected>
#include <fcntl.h>
#include <sys/stat.h>
#include <system_error>
#include <utility>

namespace oscpp_exceptionless {

class __attribute__((visibility("default"))) File {
 public:
  /**
   * Open the file at the given path.
   * @param filename Path to the file to open.
   * @param flags    Flags passed to ::open() (default O_RDONLY | O_CLOEXEC).
   * @param mode     Permission bits used if the file is created (default 0).
   */
  [[nodiscard]] static auto create(const char *filename, int flags = O_RDONLY | O_CLOEXEC, int mode = 0) noexcept
      -> std::expected<File, std::error_code>;

  File(const File &) = delete;
  File &operator=(const File &) = delete;

  /// Transfers ownership of the descriptor and any active mapping; the source becomes inert.
  File(File &&other) noexcept;
  File &operator=(File &&) = delete;

  ~File() noexcept;

  /**
   * Memory-map the file for reading. If the file is already mapped, the previous mapping is released before
   * creating the new one. An empty file maps to {nullptr, 0}.
   * @return Pointer to the mapped region and its length in bytes.
   */
  [[nodiscard]] auto map() noexcept -> std::expected<std::pair<void *, std::size_t>, std::error_code>;

  /// Get file status information for the open file.
  [[nodiscard]] auto fstat() const noexcept -> std::expected<struct stat, std::error_code>;

 private:
  explicit File(const int descriptor) noexcept : fd {descriptor} {}

  void unmap() noexcept;

  int fd;
  void *mappedFile {nullptr};
  std::size_t mappedLen {0};
};

}
#endif // OSCPP_EXCEPTIONLESS_FILE_HPP
