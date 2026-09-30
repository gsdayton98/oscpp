// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
#include <boost/test/unit_test.hpp>
#include <type_traits>
#include <utility>
#include <cerrno>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <utility>
#include "oscpp_exceptionless/file_descriptor.hpp"

// Nothing in oscpp_exceptionless may throw.
using EFD = oscpp_exceptionless::FileDescriptor;
static_assert(noexcept(EFD::create(0)));
static_assert(noexcept(std::declval<const EFD &>().clone()));
static_assert(noexcept(std::declval<EFD &>().read(nullptr, 0)) && noexcept(std::declval<EFD &>().write(nullptr, 0)));
static_assert(noexcept(std::declval<EFD &>().release()));
static_assert(std::is_nothrow_move_constructible_v<EFD> && std::is_nothrow_move_assignable_v<EFD>);

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

static auto isOpen(const int fd) -> bool { return fcntl(fd, F_GETFD) >= 0; }

BOOST_AUTO_TEST_CASE(testMoveAssignment) {
    const int first = open("/dev/null", O_RDONLY);
    const int second = open("/dev/null", O_RDONLY);
    BOOST_REQUIRE(first >= 0 && second >= 0);
    {
        auto target = oscpp_exceptionless::FileDescriptor::create(first);
        auto source = oscpp_exceptionless::FileDescriptor::create(second);
        target = std::move(source);
        // The target closed its old descriptor and took the source's.
        BOOST_CHECK(!isOpen(first));
        BOOST_CHECK_EQUAL(target.descriptor(), second);
        BOOST_CHECK(!source.valid()); // NOLINT(bugprone-use-after-move)
        BOOST_CHECK(isOpen(second));

        // Assigning an object to itself must not close its descriptor.
        auto &alias = target;
        target = std::move(alias);
        BOOST_CHECK(isOpen(second));
    }
    BOOST_CHECK(!isOpen(second));
}

BOOST_AUTO_TEST_CASE(testValidAndRelease) {
    const int sysDescriptor = open("/dev/null", O_RDONLY);
    BOOST_REQUIRE(sysDescriptor >= 0);
    auto descriptor = oscpp_exceptionless::FileDescriptor::create(sysDescriptor);
    BOOST_CHECK(descriptor.valid());
    BOOST_CHECK(static_cast<bool>(descriptor));

    const int released = descriptor.release();
    BOOST_CHECK_EQUAL(released, sysDescriptor);
    BOOST_CHECK(!descriptor.valid());
    BOOST_CHECK(!static_cast<bool>(descriptor));
    BOOST_CHECK_EQUAL(descriptor.descriptor(), -1);
    // Releasing doesn't close; the caller owns it now.
    BOOST_CHECK(isOpen(released));
    close(released);

    BOOST_CHECK(!oscpp_exceptionless::FileDescriptor::create(-1).valid());
}

BOOST_AUTO_TEST_CASE(testReadWrite) {
    int ends[2];
    BOOST_REQUIRE_EQUAL(pipe(ends), 0);
    auto reader = oscpp_exceptionless::FileDescriptor::create(ends[0]);
    auto writer = oscpp_exceptionless::FileDescriptor::create(ends[1]);

    constexpr char message[] = "hello";
    const auto written = writer.write(message, sizeof(message));
    BOOST_REQUIRE(written.has_value());
    BOOST_CHECK_EQUAL(*written, sizeof(message));

    char buffer[16] = {};
    const auto count = reader.read(buffer, sizeof(buffer));
    BOOST_REQUIRE(count.has_value());
    BOOST_CHECK_EQUAL(*count, sizeof(message));
    BOOST_CHECK_EQUAL(std::strcmp(buffer, message), 0);

    // Closing the write end makes the next read report end of file.
    writer = oscpp_exceptionless::FileDescriptor::create(-1);
    const auto eof = reader.read(buffer, sizeof(buffer));
    BOOST_REQUIRE(eof.has_value());
    BOOST_CHECK_EQUAL(*eof, 0u);
}

BOOST_AUTO_TEST_CASE(testReadWriteFail) {
    auto invalid = oscpp_exceptionless::FileDescriptor::create(-1);
    char buffer[4];
    const auto readResult = invalid.read(buffer, sizeof(buffer));
    BOOST_REQUIRE(!readResult.has_value());
    BOOST_CHECK_EQUAL(readResult.error().value(), EBADF);
    const auto writeResult = invalid.write(buffer, sizeof(buffer));
    BOOST_REQUIRE(!writeResult.has_value());
    BOOST_CHECK_EQUAL(writeResult.error().value(), EBADF);
}

BOOST_AUTO_TEST_SUITE_END()
