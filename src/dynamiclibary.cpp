// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2016 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 7/29/23.
//
#include <dlfcn.h>
#include <stdexcept>
#include "dynamiclibrary.hpp"

//  Open the current application image.
oscpp::DynamicLibrary::DynamicLibrary()
        : handle{dlopen(nullptr, 0)} {
    if (handle == nullptr) {
        throw std::runtime_error(dlerror());
    }
}


// Load the specified path into the image.
[[maybe_unused]] oscpp::DynamicLibrary::DynamicLibrary(const char *path)
        : handle{dlopen(path, 0)} {
    if (handle == nullptr) {
        throw std::runtime_error(dlerror());
    }
}


//  Find the specified symbol in the currently open library.
[[maybe_unused]] auto oscpp::DynamicLibrary::symbol(const char *symbolName) -> void * {
    void *result = dlsym(handle, symbolName);
    if (result == nullptr) {
        throw std::runtime_error(dlerror());
    }
    return result;
}


// Close the library
oscpp::DynamicLibrary::~DynamicLibrary() {
    if (handle != nullptr) {
        (void) dlclose(handle);
    }
}
