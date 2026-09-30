// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
#include <boost/test/unit_test.hpp>
#include <fcntl.h>
#include <utility>
#include "oscpp_exceptionless/socket.hpp"

BOOST_AUTO_TEST_SUITE(ExceptionlessSocket)

static auto isCloseOnExec(const int fd) -> bool {
    const int flags = fcntl(fd, F_GETFD);
    return flags >= 0 && (flags & FD_CLOEXEC) != 0;
}

BOOST_AUTO_TEST_CASE(testCreateAndClone)
{
    auto created = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE(created.has_value());
    BOOST_REQUIRE(isCloseOnExec(created->descriptor()));

    auto cloned = created->clone();
    BOOST_REQUIRE(cloned.has_value());
    BOOST_REQUIRE(isCloseOnExec(cloned->descriptor()));
    BOOST_REQUIRE(created->descriptor() != cloned->descriptor());
}

BOOST_AUTO_TEST_CASE(testCreateFails)
{
    auto failed = oscpp_exceptionless::Socket::create(-1, SOCK_STREAM, 0);
    BOOST_REQUIRE(!failed.has_value());
    BOOST_CHECK(failed.error().value() != 0);
}

BOOST_AUTO_TEST_CASE(testMove)
{
    auto original = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE(original.has_value());
    const int sysDescriptor = original->descriptor();
    {
        const oscpp_exceptionless::Socket moved{std::move(*original)};
        BOOST_CHECK_EQUAL(moved.descriptor(), sysDescriptor);
        BOOST_CHECK_EQUAL(original->descriptor(), -1); // NOLINT(bugprone-use-after-move)
        BOOST_CHECK(fcntl(sysDescriptor, F_GETFD) >= 0);
    }
    BOOST_CHECK(fcntl(sysDescriptor, F_GETFD) < 0);
}

BOOST_AUTO_TEST_SUITE_END()
