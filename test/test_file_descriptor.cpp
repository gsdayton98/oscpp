// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/15/23.
//

#include <cerrno>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_BOOST_AUTO_TEST_MODULE Test file_descriptor
#include <boost/test/unit_test.hpp>

#include <fcntl.h>
#include "file_descriptor.hpp"
#include <string>
#include "sysexception.hpp"


static auto fileDescriptorOpen(const int fd) -> bool {
    const auto sysResult = fcntl(fd, F_GETFD);
    return sysResult >= 0;
}


BOOST_AUTO_TEST_CASE(testFileDescriptor) {
    // open a file to get a file descriptor to use in tests
    auto *testFileName = "testFile.txt";

    const int sysDescriptor = open(testFileName, O_RDWR|O_CREAT|O_TRUNC, 0664);
    if (sysDescriptor < 0) {
        const std::string error = "FileDescriptor test failed setup: " + oscpp::SysException::message(errno);
        BOOST_FAIL(error.c_str());
    }
    // Check the underlying descriptor is actually open.
    BOOST_REQUIRE(fileDescriptorOpen(sysDescriptor));

    const auto fileDescriptor = oscpp::FileDescriptor::create(sysDescriptor);
    BOOST_REQUIRE_EQUAL(sysDescriptor, fileDescriptor.descriptor());

    // Clone the descriptor and check both the original and clone are open.
    int newSysDescriptor;
    {
        auto [newSocket, cloneError] = fileDescriptor.clone();
        BOOST_REQUIRE_EQUAL(cloneError, 0);
        newSysDescriptor = newSocket.descriptor();
        BOOST_REQUIRE_LT(0, newSysDescriptor);
        BOOST_REQUIRE(sysDescriptor != newSysDescriptor);
        BOOST_REQUIRE(fileDescriptorOpen(newSysDescriptor));
        BOOST_REQUIRE(fileDescriptorOpen(sysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    BOOST_REQUIRE_EQUAL(sysDescriptor, fileDescriptor.descriptor());
    BOOST_REQUIRE(!fileDescriptorOpen(newSysDescriptor));
    BOOST_REQUIRE(fileDescriptorOpen(sysDescriptor));
}
