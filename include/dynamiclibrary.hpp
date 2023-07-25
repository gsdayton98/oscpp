// Copyright 2016. Glen S. Dayton.  All rights reserved.
#include <cerrno>
#include <stdexcept>
#include <string>

namespace oscpp {

/**
 *  Get information about a dynamic library.
 */
class DynamicLibrary {
public:
  /**
   * Open the current application image.
   */
  DynamicLibrary();

  /**
   * Load the specified path into the image.
   * @param path Path to object or library
   */
  DynamicLibrary(const char *path);

  /**
   * Find the specified symbol in the currently open library.
   * @return void*  Address of the function.
   */
  void *symbol(const char *symbolName);

  /**
   * Close the library
   */
  ~DynamicLibrary();

private:
  void *handle;

  DynamicLibrary(const DynamicLibrary&) = delete;
  DynamicLibrary& operator=(const DynamicLibrary&) = delete;
};
};
