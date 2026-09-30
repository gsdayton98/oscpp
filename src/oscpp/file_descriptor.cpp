// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

//
// Created by Glen Dayton on 8/14/23.
//
// Implementation of FileDescriptor.

#include "oscpp/file_descriptor.hpp"
#include "oscpp/sysexception.hpp"


auto oscpp::FileDescriptor::create(const int descriptor) noexcept -> FileDescriptor {
    return FileDescriptor {oscpp_exceptionless::FileDescriptor::create(descriptor)};
}


[[nodiscard]] auto oscpp::FileDescriptor::clone() const -> FileDescriptor {
    auto result = impl.clone();
    if (!result) throw SysException(result.error());
    return FileDescriptor {std::move(*result)};
}


auto oscpp::FileDescriptor::read(void *buffer, const std::size_t length) -> std::size_t {
    const auto result = impl.read(buffer, length);
    if (!result) throw SysException(result.error());
    return *result;
}


auto oscpp::FileDescriptor::write(const void *buffer, const std::size_t length) -> std::size_t {
    const auto result = impl.write(buffer, length);
    if (!result) throw SysException(result.error());
    return *result;
}
