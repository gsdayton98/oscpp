// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 8/15/23.
//

#include <cerrno>

#include <boost/test/unit_test.hpp>

#include <fcntl.h>
#include "oscpp/file_descriptor.hpp"
#include <string>
#include <utility>
#include "oscpp/sysexception.hpp"
#include "temp_directory.hpp"
BOOST_AUTO_TEST_SUITE(FileDescriptor)

static auto fileDescriptorOpen(const int fd) -> bool {
    const auto sysResult = fcntl(fd, F_GETFD);
    return sysResult >= 0;
}


static auto isCloseOnExec(const int fd) -> bool {
    const int flags = fcntl(fd, F_GETFD);
    return flags >= 0 && (flags & FD_CLOEXEC) != 0;
}

BOOST_AUTO_TEST_CASE(testFileDescriptor) {
    // open a file to get a file descriptor to use in tests
    const TempDirectory directory;
    const auto testFileName = directory.path("testFile.txt");

    const int sysDescriptor = open(testFileName.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0664);
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
        auto newSocket = fileDescriptor.clone();
        newSysDescriptor = newSocket.descriptor();
        BOOST_REQUIRE_LT(0, newSysDescriptor);
        BOOST_REQUIRE(isCloseOnExec(newSysDescriptor));
        BOOST_REQUIRE(sysDescriptor != newSysDescriptor);
        BOOST_REQUIRE(fileDescriptorOpen(newSysDescriptor));
        BOOST_REQUIRE(fileDescriptorOpen(sysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    BOOST_REQUIRE_EQUAL(sysDescriptor, fileDescriptor.descriptor());
    BOOST_REQUIRE(!fileDescriptorOpen(newSysDescriptor));
    BOOST_REQUIRE(fileDescriptorOpen(sysDescriptor));
}

BOOST_AUTO_TEST_CASE(testCloneThrows) {
    const auto invalid = oscpp::FileDescriptor::create(-1);
    BOOST_CHECK_THROW((void) invalid.clone(), oscpp::SysException);
}

BOOST_AUTO_TEST_CASE(testMove) {
    const int sysDescriptor = open("/dev/null", O_RDONLY);
    BOOST_REQUIRE_LE(0, sysDescriptor);
    {
        auto original = oscpp::FileDescriptor::create(sysDescriptor);
        const oscpp::FileDescriptor moved{std::move(original)};
        BOOST_CHECK_EQUAL(moved.descriptor(), sysDescriptor);
        BOOST_CHECK_EQUAL(original.descriptor(), -1); // NOLINT(bugprone-use-after-move)
        BOOST_CHECK(fileDescriptorOpen(sysDescriptor));
    }
    // Only the final owner closes the descriptor.
    BOOST_CHECK(!fileDescriptorOpen(sysDescriptor));
}

BOOST_AUTO_TEST_SUITE_END()