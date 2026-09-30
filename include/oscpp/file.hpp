// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Wrapper around Posix file operations that protect against inadvertent copying of the file descriptor and guarantee
// release of resources.
//
// All the methods may throw an oscpp::SysException (a type of std::runtime_error) on errors.
// See oscpp_exceptionless::File for a non-throwing version.

#ifndef OSCPP_FILE_HPP
#define OSCPP_FILE_HPP
#include <cstddef>
#include <fcntl.h>
#include <sys/stat.h>
#include <utility>
#include "oscpp/sysexception.hpp"
#include "oscpp_exceptionless/file.hpp"

namespace oscpp {

/**
 * File -- open, close, memmap
 * Quite specifically, I don't provide a conversion constructor from a file descriptor because  a file descriptor is a
 * reference to a resource I don't own.
 */
class __attribute__((visibility("default"))) File {
 public:
  /**
   * Open the file at the given path.
   * @param filename Path to the file to open.
   * @param flags    Flags passed to ::open() (default O_RDONLY | O_CLOEXEC).
   * @param mode     Permission bits used if the file is created (default 0).
   * @throws oscpp::SysException if the file cannot be opened.
   */
  explicit File(const char *filename, int flags = O_RDONLY | O_CLOEXEC, int mode = 0);

  File(const File &) = delete;
  File &operator=(const File &) = delete;

  /**
   * Move constructor transfers ownership of the descriptor and any active
   * mapping, and disables the source so its destructor is a no-op.
   */
  File(File &&other) noexcept = default;

  /**
   * Release the file (mapping and descriptor) this object holds, then take ownership of the other's.
   */
  File &operator=(File &&) noexcept = default;

  ~File() = default;

  /**
   * Memory-map the file for reading. If the file is already mapped, the previous mapping is
   * released before creating the new one. An empty file maps to {nullptr, 0}.
   * @return Pointer to the mapped region and its length in bytes.
   * @throws oscpp::SysException if the file's status cannot be read or the mapping fails.
   */
  std::pair<void *, std::size_t> map();

  /**
   * Release the current memory mapping, if any. Pointers returned by map() are invalid afterward.
   */
  void unmap() noexcept { impl.unmap(); }

  /// The underlying descriptor. The file keeps ownership.
  [[nodiscard]] int descriptor() const noexcept { return impl.descriptor(); }

  /// True if this file holds a descriptor (it is not moved-from or released).
  [[nodiscard]] bool valid() const noexcept { return impl.valid(); }
  explicit operator bool() const noexcept { return valid(); }

  /// Give up ownership of the descriptor without closing it; any mapping is released first. The caller becomes
  /// responsible for closing the returned descriptor.
  [[nodiscard]] int release() noexcept { return impl.release(); }

  /**
   * Get file status information for the open file.
   * @param buffer Destination for the stat information.
   * @return The same buffer, populated.
   * @throws oscpp::SysException if the file's status cannot be read.
   */
  struct stat &fstat(struct stat &buffer) const;

 private:
  oscpp_exceptionless::File impl;
};

} // namespace oscpp
#endif // OSCPP_FILE_HPP
