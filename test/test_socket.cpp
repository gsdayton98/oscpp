// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 9/26/23.

#include <boost/test/unit_test.hpp>
#include "oscpp/socket.hpp"
#include "oscpp/sysexception.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

BOOST_AUTO_TEST_SUITE(Socket)

static auto checkSocketOpen(const int socket_fd) -> bool {
  int error = 0;
  socklen_t length = sizeof(error);
  const int sysResult = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &length);
  return sysResult >= 0 && error == 0;
}

static auto isCloseOnExec(const int fd) -> bool {
  const int flags = fcntl(fd, F_GETFD);
  return flags >= 0 && (flags & FD_CLOEXEC) != 0;
}

BOOST_AUTO_TEST_CASE(testSocket) {
  auto testSocket = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
  const auto testSysDescriptor = testSocket.descriptor();
  BOOST_REQUIRE(isCloseOnExec(testSysDescriptor));
  BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));

  // Clone the descriptor and check both the original and clone are open.
  int newSysDescriptor;
  {
    auto newSocket = testSocket.clone();
    newSysDescriptor = newSocket.descriptor();
    BOOST_REQUIRE_LT(0, newSysDescriptor);
    BOOST_REQUIRE(isCloseOnExec(newSysDescriptor));
    BOOST_REQUIRE(testSysDescriptor != newSysDescriptor);
    BOOST_REQUIRE(checkSocketOpen(newSysDescriptor));
    BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));
  }
  // Check the original is still open, and the cloned one is closed.
  BOOST_REQUIRE_EQUAL(testSysDescriptor, testSocket.descriptor());
  BOOST_REQUIRE(!checkSocketOpen(newSysDescriptor));
  BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));
}

BOOST_AUTO_TEST_CASE(testCreateThrows) {
  BOOST_CHECK_THROW(oscpp::Socket::create(-1, SOCK_STREAM, 0), oscpp::SysException);
}

BOOST_AUTO_TEST_CASE(testMove) {
  int sysDescriptor;
  {
    auto original = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    sysDescriptor = original.descriptor();
    const oscpp::Socket moved{std::move(original)};
    BOOST_CHECK_EQUAL(moved.descriptor(), sysDescriptor);
    BOOST_CHECK_EQUAL(original.descriptor(), -1); // NOLINT(bugprone-use-after-move)
    BOOST_CHECK(checkSocketOpen(sysDescriptor));
  }
  // Only the final owner closes the descriptor.
  BOOST_CHECK(!checkSocketOpen(sysDescriptor));
}

BOOST_AUTO_TEST_CASE(testMoveAssignment) {
  auto first = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
  const int firstDescriptor = first.descriptor();
  {
    auto second = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    const int secondDescriptor = second.descriptor();
    first = std::move(second);
    BOOST_CHECK(!checkSocketOpen(firstDescriptor));
    BOOST_CHECK_EQUAL(first.descriptor(), secondDescriptor);
    BOOST_CHECK(!second.valid()); // NOLINT(bugprone-use-after-move)
  }
  BOOST_CHECK(checkSocketOpen(first.descriptor()));
}

BOOST_AUTO_TEST_CASE(testValidAndRelease) {
  auto socket = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
  BOOST_CHECK(socket);
  const int sysDescriptor = socket.descriptor();
  BOOST_CHECK_EQUAL(socket.release(), sysDescriptor);
  BOOST_CHECK(!socket);
  BOOST_CHECK(checkSocketOpen(sysDescriptor));
  close(sysDescriptor);
}

BOOST_AUTO_TEST_SUITE_END()
