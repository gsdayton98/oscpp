// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

//
// Created by Glen Dayton on 8/14/23.
//
// Implementation of FileDescriptor.

#include <cerrno>
#include "file_descriptor.hpp"
#include <unistd.h>


[[maybe_unused]] oscpp::FileDescriptor::FileDescriptor(oscpp::FileDescriptor && original) noexcept
: handle {original.handle}
{
    original.handle = -1;
}


oscpp::FileDescriptor::~FileDescriptor() noexcept {
    if (-1 < handle) {
        ::close(handle);
    }
}


[[maybe_unused]]
auto oscpp::FileDescriptor::create(int descriptor) noexcept -> FileDescriptor {return std::move(FileDescriptor {descriptor});}


[[maybe_unused]] [[nodiscard]] auto oscpp::FileDescriptor::clone() const noexcept -> std::pair<FileDescriptor, int> {
    int errorCode = 0;
    auto newSysDescriptor = ::dup(handle);
    if (newSysDescriptor < 0) {
        errorCode = errno;
    }
    return std::make_pair(std::move(FileDescriptor {newSysDescriptor}), errorCode);
}
