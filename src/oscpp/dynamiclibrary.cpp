// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 7/29/23.
//
// Copyright 2016. Glen S. Dayton. Rights reserved according to terms of included license.
#include <stdexcept>
#include "oscpp/dynamiclibrary.hpp"

namespace {
template <typename T>
auto unwrap(std::expected<T, std::string> &&result) -> T {
    if (!result) throw std::runtime_error(result.error());
    return std::move(*result);
}
}

//  Open the current application image.
oscpp::DynamicLibrary::DynamicLibrary()
        : impl{unwrap(oscpp_exceptionless::DynamicLibrary::create())} {}


// Load the specified path into the image.
[[maybe_unused]] oscpp::DynamicLibrary::DynamicLibrary(const char *path)
        : impl{unwrap(oscpp_exceptionless::DynamicLibrary::create(path))} {}


//  Find the specified symbol in the currently open library.
[[maybe_unused]] auto oscpp::DynamicLibrary::symbol(const char *symbolName) const -> void * {
    return unwrap(impl.symbol(symbolName));
}
