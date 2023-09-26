// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Created by Glen Dayton on 9/26/23.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_BOOST_AUTO_TEST_MODULE Test socket
#include <boost/test/unit_test.hpp>
#include "socket.hpp"
#include <sys/socket.h>

static auto checkSocketOpen(int socket_fd) -> bool {
    int error = 0;
    socklen_t length = sizeof (error);
    int sysResult = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &length);
    return sysResult >= 0 && error == 0;
}



BOOST_AUTO_TEST_CASE(testSocket)
{
    auto createResult = oscpp::Socket::create(PF_LOCAL, SOCK_STREAM, 0);
    BOOST_CHECK_EQUAL(createResult.second, 0);
    auto testSocket = std::move(createResult.first);
    auto testSysDescriptor = testSocket.descriptor();
    BOOST_CHECK(checkSocketOpen(testSysDescriptor));

    // Clone the descriptor and check both the original and clone are open.
    int newSysDescriptor;
    {
        auto newDescriptor = testSocket.clone();
        BOOST_CHECK_EQUAL(newDescriptor.second, 0);
        newSysDescriptor = newDescriptor.first.descriptor();
        BOOST_CHECK_LT(0, newSysDescriptor);
        BOOST_CHECK(testSysDescriptor != newSysDescriptor);
        BOOST_CHECK(checkSocketOpen(newSysDescriptor));
        BOOST_CHECK(checkSocketOpen(testSysDescriptor));
    }
    // Check the original is still open, and the cloned one is closed.
    BOOST_CHECK_EQUAL(testSysDescriptor, testSocket.descriptor());
    BOOST_CHECK(!checkSocketOpen(newSysDescriptor));
    BOOST_CHECK(checkSocketOpen(testSysDescriptor));
}

#pragma clang diagnostic pop