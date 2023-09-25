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


static auto fileDescriptorOpen(int fd) -> bool {
    auto sysResult = ::fcntl(fd, F_GETFD);
    return sysResult >= 0;
}


TEST(oscpp, testFileDescriptor) {
    // open a file to get a file descriptor to use in tests
    const char *testFileName = "testFile.txt";

    int sysDescriptor = ::open(testFileName, O_RDWR|O_CREAT|O_TRUNC, 0664);
    if (sysDescriptor < 0) {
        std::string error = "FileDescriptor test failed setup: " + oscpp::SysException::message(errno);
        FAIL(error.c_str());
    }
    // Check the underlying descriptor is actually open.
    CHECK(fileDescriptorOpen(sysDescriptor));

    auto fileDescriptor = oscpp::FileDescriptor::create(sysDescriptor);
    CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());

    // Clone the descriptor and check both the original and clone are open.
    int newSysDescriptor = -1;
    {
        auto newDescriptor = fileDescriptor.clone();
        newSysDescriptor = newDescriptor.descriptor();
        CHECK_LT(0, newSysDescriptor);
        CHECK(sysDescriptor != newSysDescriptor);
        CHECK(fileDescriptorOpen(newSysDescriptor));
        CHECK(fileDescriptorOpen(sysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());
    CHECK(!fileDescriptorOpen(newSysDescriptor));
    CHECK(fileDescriptorOpen(sysDescriptor));
}