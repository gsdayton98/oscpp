// -*- mode: c++ -*-
// @copyright  2021 Glen S. Dayton. All rights reserved. Project Euler has confidentiality rules so do not copy nor publish this code.
//  @author Glen S.Dayton

#include "file.hpp"
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>


std::pair<void*, size_t> oscpp::File::map() {
  struct stat stats{};

  fstat(stats);

  if (mappedFile) {
    (void) munmap( mappedFile,  mappedLen);
  }

  mappedLen = stats.st_size;
  mappedFile = mmap(nullptr, mappedLen, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0L);
  if (mappedFile == MAP_FAILED) {
    mappedFile = nullptr;
    throw SysException{};
  }

  return std::make_pair(mappedFile, mappedLen);
}


struct stat& oscpp::File::fstat(struct stat& buffer) const {
  if (::fstat(fd, &buffer) < 0) throw SysException{};
  return buffer;
}


int oscpp::File::open(const char* filename, const int flags, const int mode) {
  const int fileDescriptor = ::open(filename, flags, mode);
  if (fileDescriptor < 0) throw SysException{};
  return fileDescriptor;
}


void oscpp::File::close() {
  if (mappedFile) {
    (void) munmap( mappedFile,  mappedLen);
    mappedFile = nullptr;
    mappedLen = 0;
  }
  if (fd >= 0) {
    (void)::close(fd);
    fd = -1;
  }
}

