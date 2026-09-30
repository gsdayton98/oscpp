// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
#include <boost/test/unit_test.hpp>
#include <cerrno>
#include <fcntl.h>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"

BOOST_AUTO_TEST_SUITE(ExceptionlessFileDescriptor)

BOOST_AUTO_TEST_CASE(testClone) {
    const auto fileDescriptor = oscpp_exceptionless::FileDescriptor::create(open("/dev/null", O_RDONLY));
    auto cloned = fileDescriptor.clone();
    BOOST_REQUIRE(cloned.has_value());
    BOOST_REQUIRE(cloned->descriptor() != fileDescriptor.descriptor());
    const int flags = fcntl(cloned->descriptor(), F_GETFD);
    BOOST_REQUIRE(flags >= 0 && (flags & FD_CLOEXEC) != 0);
}

BOOST_AUTO_TEST_CASE(testCloneFails) {
    const auto invalid = oscpp_exceptionless::FileDescriptor::create(-1);
    auto failed = invalid.clone();
    BOOST_REQUIRE(!failed.has_value());
    BOOST_CHECK_EQUAL(failed.error().value(), EBADF);
}

BOOST_AUTO_TEST_CASE(testMove) {
    const int sysDescriptor = open("/dev/null", O_RDONLY);
    BOOST_REQUIRE_LE(0, sysDescriptor);
    {
        auto original = oscpp_exceptionless::FileDescriptor::create(sysDescriptor);
        const oscpp_exceptionless::FileDescriptor moved{std::move(original)};
        BOOST_CHECK_EQUAL(moved.descriptor(), sysDescriptor);
        BOOST_CHECK_EQUAL(original.descriptor(), -1); // NOLINT(bugprone-use-after-move)
        BOOST_CHECK(fcntl(sysDescriptor, F_GETFD) >= 0);
    }
    BOOST_CHECK(fcntl(sysDescriptor, F_GETFD) < 0);
}

BOOST_AUTO_TEST_SUITE_END()
