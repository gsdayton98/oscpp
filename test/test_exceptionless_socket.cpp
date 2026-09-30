// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
#include <boost/test/unit_test.hpp>
#include <fcntl.h>
#include <unistd.h>
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

// The socket owns a FileDescriptor, which shares its descriptor and can be cloned independently.
BOOST_AUTO_TEST_CASE(testFileDescriptorAccessor)
{
    auto created = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE(created.has_value());
    BOOST_CHECK_EQUAL(created->fileDescriptor().descriptor(), created->descriptor());

    const auto duplicate = created->fileDescriptor().clone();
    BOOST_REQUIRE(duplicate.has_value());
    BOOST_CHECK(duplicate->descriptor() != created->descriptor());
}

BOOST_AUTO_TEST_CASE(testMoveAssignment)
{
    auto first = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    auto second = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE(first.has_value() && second.has_value());
    const int firstDescriptor = first->descriptor();
    const int secondDescriptor = second->descriptor();

    *first = std::move(*second);
    BOOST_CHECK(fcntl(firstDescriptor, F_GETFD) < 0);
    BOOST_CHECK_EQUAL(first->descriptor(), secondDescriptor);
    BOOST_CHECK(!second->valid()); // NOLINT(bugprone-use-after-move)
}

BOOST_AUTO_TEST_CASE(testValidAndRelease)
{
    auto created = oscpp_exceptionless::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE(created.has_value());
    BOOST_CHECK(created->valid());
    BOOST_CHECK(static_cast<bool>(*created));

    const int sysDescriptor = created->descriptor();
    BOOST_CHECK_EQUAL(created->release(), sysDescriptor);
    BOOST_CHECK(!created->valid());
    BOOST_CHECK(fcntl(sysDescriptor, F_GETFD) >= 0);
    close(sysDescriptor);
}

BOOST_AUTO_TEST_SUITE_END()
