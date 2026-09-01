// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016. Glen S. Dayton. Rights reserved according to terms of included license.
#ifndef OSCPP_DYNAMIC_LIBRARY_HPP
#define OSCPP_DYNAMIC_LIBRARY_HPP

namespace oscpp {
/**
 *  Get information about a dynamic library.
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
        [[maybe_unused]] [[maybe_unused]]
        explicit DynamicLibrary(const char *path);

        /**
         * Find the specified symbol in the currently open library.
         * @return void*  Address of the function.
         * @throws std::runtime_error if the symbol cannot be found.
         */
        [[maybe_unused]] [[maybe_unused]]
        auto symbol(const char *symbolName) const -> void *;

        /**
         * Close the library
         */
        ~DynamicLibrary();

        DynamicLibrary(const DynamicLibrary &) = delete;

        DynamicLibrary &operator=(const DynamicLibrary &) = delete;

    private:
        void *handle;
    };
}

#endif // OSCPP_DYNAMIC_LIBRARY_HPP
