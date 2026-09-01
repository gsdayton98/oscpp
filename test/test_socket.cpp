// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 9/26/23.
//

#define BOOST_BOOST_AUTO_TEST_MODULE Test socket
#include <boost/test/unit_test.hpp>
#include "socket.hpp"
#include <sys/socket.h>

static auto checkSocketOpen(const int socket_fd) -> bool {
    int error = 0;
    socklen_t length = sizeof (error);
    const int sysResult = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &length);
    return sysResult >= 0 && error == 0;
}



BOOST_AUTO_TEST_CASE(testSocket)
{
    auto [testSocket, createError] = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_REQUIRE_EQUAL(createError, 0);
    const auto testSysDescriptor = testSocket.descriptor();
    BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));

    // Clone the descriptor and check both the original and clone are open.
    int newSysDescriptor;
    {
        auto [newSocket, cloneError] = testSocket.clone();
        BOOST_REQUIRE_EQUAL(cloneError, 0);
        newSysDescriptor = newSocket.descriptor();
        BOOST_REQUIRE_LT(0, newSysDescriptor);
        BOOST_REQUIRE(testSysDescriptor != newSysDescriptor);
        BOOST_REQUIRE(checkSocketOpen(newSysDescriptor));
        BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    BOOST_REQUIRE_EQUAL(testSysDescriptor, testSocket.descriptor());
    BOOST_REQUIRE(!checkSocketOpen(newSysDescriptor));
    BOOST_REQUIRE(checkSocketOpen(testSysDescriptor));
}
