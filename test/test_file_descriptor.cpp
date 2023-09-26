// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/15/23.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#include <cerrno>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_BOOST_AUTO_TEST_MODULE Test file_descriptor
#include <boost/test/unit_test.hpp>

#include <fcntl.h>
#include "file_descriptor.hpp"
#include <string>
#include "sysexception.hpp"


static auto fileDescriptorOpen(int fd) -> bool {
    auto sysResult = ::fcntl(fd, F_GETFD);
    return sysResult >= 0;
}


BOOST_AUTO_TEST_CASE(testFileDescriptor) {
    // open a file to get a file descriptor to use in tests
    const char *testFileName = "testFile.txt";

    int sysDescriptor = ::open(testFileName, O_RDWR|O_CREAT|O_TRUNC, 0664);
    if (sysDescriptor < 0) {
        std::string error = "FileDescriptor test failed setup: " + oscpp::SysException::message(errno);
        BOOST_FAIL(error.c_str());
    }
    // Check the underlying descriptor is actually open.
    BOOST_CHECK(fileDescriptorOpen(sysDescriptor));

    auto fileDescriptor = oscpp::FileDescriptor::create(sysDescriptor);
    BOOST_CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());

    // Clone the descriptor and check both the original and clone are open.
    int newSysDescriptor;
    {
        auto newDescriptor = fileDescriptor.clone();
        BOOST_CHECK_EQUAL(newDescriptor.second, 0);
        newSysDescriptor = newDescriptor.first.descriptor();
        BOOST_CHECK_LT(0, newSysDescriptor);
        BOOST_CHECK(sysDescriptor != newSysDescriptor);
        BOOST_CHECK(fileDescriptorOpen(newSysDescriptor));
        BOOST_CHECK(fileDescriptorOpen(sysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    BOOST_CHECK_EQUAL(sysDescriptor, fileDescriptor.descriptor());
    BOOST_CHECK(!fileDescriptorOpen(newSysDescriptor));
    BOOST_CHECK(fileDescriptorOpen(sysDescriptor));
}
#pragma clang diagnostic pop