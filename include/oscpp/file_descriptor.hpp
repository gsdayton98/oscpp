// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
////
// OSCPP File Descriptor
// Copyright 2023 Glen S. Dayton. Rights reserved according to the included license terms.
// Created by Glen Dayton on 8/14/23.
//
// Captures the use semantics of a typical handle or file descriptor.  Prohibits copying but provides a clone()
// method for duplicating the descriptor.  Closes the descriptor on destruction.

#ifndef OSCPP_FILE_DESCRIPTOR_HPP
#define OSCPP_FILE_DESCRIPTOR_HPP
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"


namespace oscpp {

    class __attribute__((visibility("default"))) FileDescriptor {
        /**
         * Implementation-dependent file handle or descriptor.
         */
        oscpp_exceptionless::FileDescriptor impl;

        /**
         * Use the create() method to create new file descriptors
         * @param fileDescriptor
         */
        explicit FileDescriptor(oscpp_exceptionless::FileDescriptor &&descriptor) noexcept : impl {std::move(descriptor)} {}

    public:

        static auto create(int descriptor) noexcept -> FileDescriptor;

        /**
         * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
         */
        FileDescriptor(const FileDescriptor &) = delete;

        /**
         * Move constructor creates a new FileDescriptor with the same handle and disables the old handle to prevent
         * it from getting closed.
         */
        FileDescriptor(FileDescriptor &&) noexcept = default;

        /**
         * Close the descriptor.
         */
        ~FileDescriptor() noexcept = default;

        /**
         * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
         */
        FileDescriptor &operator=(const FileDescriptor &) = delete;

        /**
          * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
          * Move version of the assignment operator doesn't make sense.
          */
        FileDescriptor &operator=(FileDescriptor &&) = delete;

        /**
         * Duplicate the existing FileDescriptor into a new FileDescriptor. The new descriptor is close-on-exec.
         * @return The new FileDescriptor.
         * @throws oscpp::SysException on failure. See oscpp_exceptionless::FileDescriptor::clone for a non-throwing version.
         */
        [[nodiscard]] auto clone() const -> FileDescriptor;

        /**
         * Return the low-level implementation specific file descriptor.
         * @return Operating system file handle
         */
        [[nodiscard]] auto descriptor() const noexcept -> int { return impl.descriptor(); }
    };
}
#endif //OSCPP_FILE_DESCRIPTOR_HPP
