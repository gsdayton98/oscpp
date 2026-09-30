// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016. Glen S. Dayton. Rights reserved according to terms of included license.
#ifndef OSCPP_DYNAMIC_LIBRARY_HPP
#define OSCPP_DYNAMIC_LIBRARY_HPP
#include <utility>
#include "oscpp_exceptionless/dynamiclibrary.hpp"

namespace oscpp {
/**
 *  Get information about a dynamic library. See oscpp_exceptionless::DynamicLibrary for a non-throwing version.
 */
    class __attribute__((visibility("default"))) DynamicLibrary {
    public:
        /**
         * Open the current application image.
         *
         * @param flags dlopen() flags (default RTLD_NOW | RTLD_LOCAL).
         * @throws std::runtime_error if this operation fails.
         * DynamicLibrary errors are std::runtime_errors, not SysExceptions, because the message comes from
         * dlerror() and there is no errno to report.
         */
        explicit DynamicLibrary(int flags = oscpp_exceptionless::DynamicLibrary::DefaultFlags);

        /**
         * Load the specified path into the image.
         * @param path  Path to object or library
         * @param flags dlopen() flags (default RTLD_NOW | RTLD_LOCAL).
         *
         * @throws std::runtime_error if this operation fails (see above).
         */
        explicit DynamicLibrary(const char *path, int flags = oscpp_exceptionless::DynamicLibrary::DefaultFlags);

        /**
         * Find the specified symbol in the currently open library.
         * @return void*  Address of the function.
         * @throws std::runtime_error if the symbol cannot be found.
         */
        auto symbol(const char *symbolName) const -> void *;

        /**
         * Close the library
         */
        ~DynamicLibrary() = default;

        DynamicLibrary(DynamicLibrary &&) noexcept = default;

        DynamicLibrary(const DynamicLibrary &) = delete;

        DynamicLibrary &operator=(const DynamicLibrary &) = delete;

    private:
        oscpp_exceptionless::DynamicLibrary impl;
    };
}

#endif // OSCPP_DYNAMIC_LIBRARY_HPP
