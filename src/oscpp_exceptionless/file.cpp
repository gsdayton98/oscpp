// -*- mode: c++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
#include "oscpp_exceptionless/file.hpp"

namespace {
auto lastError() noexcept -> std::error_code { return {errno, std::generic_category()}; }
}

auto oscpp_exceptionless::File::create(const char *filename, const int flags, const int mode) noexcept
    -> std::expected<File, std::error_code> {
  const int descriptor = ::open(filename, flags, mode);
  if (descriptor < 0) return std::unexpected(lastError());
  return File {descriptor};
}

oscpp_exceptionless::File::File(File &&other) noexcept
: fd {other.fd},
  mappedFile {other.mappedFile},
  mappedLen {other.mappedLen}
{
  other.fd = -1;
  other.mappedFile = nullptr;
  other.mappedLen = 0;
}

oscpp_exceptionless::File::~File() noexcept { close(); }

void oscpp_exceptionless::File::close() noexcept {
  unmap();
  if (fd >= 0) {
    (void) ::close(fd);
    fd = -1;
  }
}

auto oscpp_exceptionless::File::operator=(File &&other) noexcept -> File & {
  if (this != &other) {
    close();
    fd = other.fd;
    mappedFile = other.mappedFile;
    mappedLen = other.mappedLen;
    other.fd = -1;
    other.mappedFile = nullptr;
    other.mappedLen = 0;
  }
  return *this;
}

auto oscpp_exceptionless::File::release() noexcept -> int {
  unmap();
  const int descriptor = fd;
  fd = -1;
  return descriptor;
}

void oscpp_exceptionless::File::unmap() noexcept {
  if (mappedFile) {
    (void) munmap(mappedFile, mappedLen);
    mappedFile = nullptr;
    mappedLen = 0;
  }
}

auto oscpp_exceptionless::File::fstat() const noexcept -> std::expected<struct stat, std::error_code> {
  struct stat buffer {};
  if (::fstat(fd, &buffer) < 0) return std::unexpected(lastError());
  return buffer;
}

auto oscpp_exceptionless::File::map() noexcept -> std::expected<std::pair<void *, std::size_t>, std::error_code> {
  const auto stats = fstat();
  if (!stats) return std::unexpected(stats.error());

  unmap();

  // mmap() rejects a zero length with EINVAL, so an empty file maps to nothing.
  const auto length = static_cast<std::size_t>(stats->st_size);
  if (length == 0) return std::pair<void *, std::size_t> {nullptr, 0};

  void *const region = mmap(nullptr, length, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0L);
  if (region == MAP_FAILED) return std::unexpected(lastError());

  mappedFile = region;
  mappedLen = length;
  return std::pair<void *, std::size_t> {mappedFile, mappedLen};
}
