// -*- mode: c++ -*-
////
// @copyright 2021 Glen S. Dayton. All rights reserved. Project Euler has confidentiality rules, so do not copy nor publish this code.
// @author Glen S. Dayton
//
//  Wrapper around Posix file operations that protect against inadvertent copying of the file descriptor and guarantee
//  release of resources.
//
// All the methods may throw an oscpp::SysException (a type of std::runtime_error) on errors.

#ifndef FILE_H_INCL
#define FILE_H_INCL
#include <fcntl.h>
#include <sys/stat.h>
#include "sysexception.hpp"

namespace oscpp {

/**
 * File -- open, close, memmap
 * Quite specifically, I don't provide a conversion constructor from a file descriptor because  a file descriptor is a
 * reference to a resource I don't own.
 */
class File {
 public:
  explicit File(const char *filename, const int flags = O_RDONLY | O_CLOEXEC, const int mode = 0)
  : fd {open(filename, flags, mode)},
        mappedFile {nullptr},
        mappedLen {0}
  {}


  File(const File&) = delete;
  File& operator=(const File&) = delete;
  ~File() { close(); }

  std::pair<void*, size_t> map();
  struct stat& fstat(struct stat& buffer) const;

 private:
  int fd;
  void *mappedFile;
  size_t mappedLen;

  static int open(const char *filename, int flags = O_RDONLY | O_CLOEXEC, int mode = 0);
  void close();
};

}
#endif // FILE_H_INCL
