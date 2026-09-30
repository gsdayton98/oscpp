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
    class [[maybe_unused]] __attribute__((visibility("default"))) DynamicLibrary {
    public:
        /**
         * Open the current application image.
         *
         * @throws std::runtime_error if this operation fails.
         * DynamicLibrary runtime_errors are not SystemExceptions because SystemExceptions get their messages
         * from strerror_r(), but DynamicLibrary gets its messages from dlerror().
         */
        [[maybe_unused]]
        DynamicLibrary();

        /**
         * Load the specified path into the image.
         * @param path Path to object or library
         *
         * @throws std::runtime_error if this operation fails.
         * DynamicLibrary runtime_errors are not SystemExceptions because SystemExceptions get their messages
         * from strerror_r(), but DynamicLibrary gets its messages from dlerror().
         */
        [[maybe_unused]]
        explicit DynamicLibrary(const char *path);

        /**
         * Find the specified symbol in the currently open library.
         * @return void*  Address of the function.
         * @throws std::runtime_error if the symbol cannot be found.
         */
        [[maybe_unused]]
        auto symbol(const char *symbolName) const -> void *;

        /**
         * Close the library
         */
        ~DynamicLibrary() = default;

        [[maybe_unused]] DynamicLibrary(DynamicLibrary &&) noexcept = default;

        DynamicLibrary(const DynamicLibrary &) = delete;

        DynamicLibrary &operator=(const DynamicLibrary &) = delete;

    private:
        oscpp_exceptionless::DynamicLibrary impl;
    };
}

#endif // OSCPP_DYNAMIC_LIBRARY_HPP
