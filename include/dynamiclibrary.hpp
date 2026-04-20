// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016. Glen S. Dayton. Rights reserved according to terms of included license.

namespace oscpp {

/**
 *  Get information about a dynamic library.
 */
    class [[maybe_unused]] DynamicLibrary {
    public:
        /**
         * Open the current application image.
         */
        [[maybe_unused]]
        DynamicLibrary();

        /**
         * Load the specified path into the image.
         * @param path Path to object or library
         */
        [[maybe_unused]] [[maybe_unused]]
        explicit DynamicLibrary(const char *path);

        /**
         * Find the specified symbol in the currently open library.
         * @return void*  Address of the function.
         */
        [[maybe_unused]] [[maybe_unused]]
        auto symbol(const char *symbolName) -> void *;

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
