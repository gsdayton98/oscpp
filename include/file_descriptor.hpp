// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
////
// OSCPP File Descriptor
// Copyright 2023 Glen S. Dayton. Rights reserved according to the included license terms.
// Created by Glen Dayton on 8/14/23.
//
// Captures the use semantics of a typical handle or file descriptor.  Prohibits copying, but provides a clone()
// method for duplicating the descriptor.  Closes the descriptor on destruction.

#ifndef OSCPP_FILE_DESCRIPTOR_HPP
#define OSCPP_FILE_DESCRIPTOR_HPP
#include <utility>


namespace oscpp {

    class [[maybe_unused]] FileDescriptor {
    private:
        /**
         * Implementation dependent file handle or descriptor.
         */
        int handle;

        /**
         * Use the create() method to create new file descriptors
         * @param fileDescriptor
         */
        [[maybe_unused]] explicit FileDescriptor(int fileDescriptor) noexcept : handle {fileDescriptor} {}

    public:

        [[maybe_unused]] static auto create(int descriptor) noexcept -> FileDescriptor;

        /**
         * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
         */
        FileDescriptor(FileDescriptor &) = delete;

        /**
         * Move constructor creates a new FileDescriptor with the same handle, the disables the old handle to prevent
         * it from getting closed.
         */
        [[maybe_unused]] FileDescriptor(FileDescriptor &&) noexcept;

        /**
         * Close the descriptor.
         */
        ~FileDescriptor() noexcept;

        /**
         * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
         */
        FileDescriptor &operator=(FileDescriptor &) = delete;

        /**
          * Cannot copy a file descriptor.  Use the clone method to duplicate the descriptor into a new descriptor.
          * Move version of the assignment operator doesn't make sense.
          */
        FileDescriptor &operator=(FileDescriptor &&) = delete;

        /**
         * Duplicate the existing FileDescriptor into a new FileDescriptor.
         * @return A new (FileDescriptor, error code) pair. Do not use the File Descriptor is non-zero.
         */
        [[maybe_unused]] [[nodiscard]] auto clone() const noexcept -> std::pair<FileDescriptor, int>;

        /**
         * Return the low-level implementation specific file descriptor.
         * @return Operating system file handle
         */
        [[maybe_unused]] [[nodiscard]] auto descriptor() const noexcept -> int { return handle; }
    };
}
#endif //OSCPP_FILE_DESCRIPTOR_HPP
