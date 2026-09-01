// -*- mode: c++ -*-
////
// @copyright 2021 Glen S. Dayton. Rights reserved according to terms of included license.
// @author Glen S. Dayton
//
//  Wrapper around Posix file operations that protect against inadvertent copying of the file descriptor and guarantee
//  release of resources.
//
// All the methods may throw an oscpp::SysException (a type of std::runtime_error) on errors.

#ifndef OSCPP_FILE_HPP
#define OSCPP_FILE_HPP
#include <fcntl.h>
#include <sys/stat.h>
#include "sysexception.hpp"

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
  explicit File(const char *filename, const int flags = O_RDONLY | O_CLOEXEC, const int mode = 0)
  : fd {open(filename, flags, mode)},
        mappedFile {nullptr},
        mappedLen {0}
  {}


  File(const File&) = delete;
  File& operator=(const File&) = delete;

  /**
   * Move constructor transfers ownership of the descriptor and any active
   * mapping, and disables the source so its destructor is a no-op.
   */
  File(File&& other) noexcept
  : fd {other.fd},
        mappedFile {other.mappedFile},
        mappedLen {other.mappedLen}
  {
    other.fd = -1;
    other.mappedFile = nullptr;
    other.mappedLen = 0;
  }

  /**
   * Move version of the assignment operator doesn't make sense.
   */
  File& operator=(File&&) = delete;

  ~File() { close(); }

  /**
   * Memory-map the file for reading. If the file is already mapped, the previous mapping is
   * released before creating the new one.
   * @return Pointer to the mapped region and its length in bytes.
   * @throws oscpp::SysException if the file's status cannot be read or the mapping fails.
   */
  std::pair<void*, size_t> map();

  /**
   * Get file status information for the open file.
   * @param buffer Destination for the stat information.
   * @return The same buffer, populated.
   * @throws oscpp::SysException if the file's status cannot be read.
   */
  struct stat& fstat(struct stat& buffer) const;

 private:
  int fd;
  void *mappedFile;
  size_t mappedLen;

  static int open(const char *filename, int flags = O_RDONLY | O_CLOEXEC, int mode = 0);
  void close();
};

}
#endif // OSCPP_FILE_HPP
