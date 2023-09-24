// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

//
// Created by Glen Dayton on 8/15/23.
//
#include <cerrno>
#include <CppUnitXLite/CppUnitXLite.hpp>
#include <fcntl.h>
#include "file_descriptor.hpp"
#include <string>
#include "sysexception.hpp"

TEST(oscpp, testFileDescriptor) {
    // open a file to get a file descriptor to use in tests
    const char *testFileName = "testFile.txt";

    int sysDescriptor = ::open(testFileName, O_RDWR|O_CREAT|O_TRUNC, 0664);
    if (sysDescriptor < 0) {
        std::string error = "FileDescriptor test failed setup: " + oscpp::SysException::message(errno);
        FAIL(error.c_str());
    }

    auto fileDescriptor = oscpp::FileDescriptor::create(sysDescriptor);
    CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());

    {
        auto newDescriptor = fileDescriptor.clone();
        CHECK_LT(0, newDescriptor.descriptor());
        CHECK(fileDescriptor.descriptor() != newDescriptor.descriptor());
    }
    CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());
}