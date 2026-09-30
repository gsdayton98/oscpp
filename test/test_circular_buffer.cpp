// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
#include <boost/test/unit_test.hpp>

#include "oscpp/circular_buffer.hpp"

BOOST_AUTO_TEST_SUITE(CircularBuffer)

namespace {
    // Inherit from CircularBuffer, so we can test the internal functions.
    struct TesterCircularBuffer : oscpp::CircularBuffer<uintptr_t> {
        static size_t roundup(const size_t n) { return CircularBuffer::roundup(n); }

        explicit TesterCircularBuffer(const size_t n) : CircularBuffer{n} {}

        [[nodiscard]] auto next(const size_t i) const -> size_t  { return CircularBuffer::next(i); }

    };


    struct TestCase {
        size_t arg;
        size_t expected;

        TestCase(const size_t theArg, const size_t theExpected)
                : arg(theArg), expected(theExpected) {}
    };


}


BOOST_AUTO_TEST_CASE(Roundup) {
    TestCase testCases[] = {
            TestCase{0, 4},
            TestCase{1, 4},
            TestCase{2, 4},
            TestCase{3, 4},
            TestCase{4, 4},
            TestCase{5, 8},
            TestCase{7, 8},
            TestCase{8, 8},
            TestCase{60, 64}
    };

    for (const TestCase &aTestCase: testCases) {
        BOOST_CHECK_EQUAL(aTestCase.expected, TesterCircularBuffer::roundup(aTestCase.arg));
    }
}

BOOST_AUTO_TEST_CASE(Construction) {
    const oscpp::CircularBuffer<uintptr_t> buffer{7};

    BOOST_CHECK_EQUAL(7UL, buffer.capacity());
}

BOOST_AUTO_TEST_CASE(Next) {
    const TesterCircularBuffer buffer{8};

    for (size_t i = 0; i < 2 * buffer.capacity(); ++i) {
        const size_t j = buffer.next(i);
        BOOST_CHECK_EQUAL((i + 1UL) % (buffer.capacity() + 1), j);
    }
}

BOOST_AUTO_TEST_CASE(Zero) {
    const oscpp::CircularBuffer<uintptr_t> buffer{0};

    // A requested capacity of 0 is floored to MinSize, so the buffer is
    // empty but not full -- this is what keeps put()/get() from deadlocking
    // forever on a degenerate zero-capacity request.
    BOOST_CHECK(buffer.empty());
    BOOST_CHECK(!buffer.full());
    BOOST_CHECK_EQUAL(oscpp::CircularBuffer<uintptr_t>::MinSize, buffer.capacity() + 1);
}

BOOST_AUTO_TEST_CASE(Size) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        BOOST_CHECK(buffer.tryPut(x));
    }
    BOOST_CHECK_EQUAL(5UL, buffer.size());
}

BOOST_AUTO_TEST_CASE(Full)
{
    constexpr size_t TESTSIZE = 8u;
    oscpp::CircularBuffer<int> buffer{TESTSIZE};

    BOOST_CHECK_EQUAL(buffer.capacity() + 1, TESTSIZE);
    BOOST_CHECK(buffer.empty());
    BOOST_CHECK(!buffer.full());

    for (int n = 0; n < static_cast<int>(TESTSIZE) - 1; ++n) {
        BOOST_CHECK(buffer.tryPut(n));
    }
    BOOST_CHECK(buffer.full());
}

BOOST_AUTO_TEST_CASE(ReadWrite1) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        BOOST_CHECK(buffer.tryPut(x));
    }


    uintptr_t justRead;
    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        BOOST_CHECK(buffer.tryGet(justRead));
        BOOST_CHECK_EQUAL(x, justRead);
    }
}

BOOST_AUTO_TEST_CASE(ReadWriteX) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    uintptr_t expectedRead = 1UL;
    uintptr_t justRead;
    for (uintptr_t x = 1UL; x <= 25UL; ++x) {
        while (!buffer.tryPut(x)) {
            BOOST_CHECK(buffer.tryGet(justRead));
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            BOOST_CHECK(buffer.tryGet(justRead));
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
            BOOST_CHECK(buffer.tryGet(justRead));
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
        }
    }

    while (buffer.tryGet(justRead)) {
        BOOST_CHECK_EQUAL(expectedRead, justRead);
        ++expectedRead;
    }
    BOOST_CHECK(buffer.empty());
}


BOOST_AUTO_TEST_CASE(Singlethread) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    uintptr_t expectedRead = 1UL;
    uintptr_t justRead;

    for (uintptr_t x = 1UL; x <= 25UL; ++x) {
        buffer.put(x);
        while (buffer.full()) {
            justRead = buffer.get();
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            justRead = buffer.get();
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            justRead = buffer.get();
            BOOST_CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
        }
    }

    while (!buffer.empty()) {
        justRead = buffer.get();
        BOOST_CHECK_EQUAL(expectedRead, justRead);
        ++expectedRead;
    }
    BOOST_CHECK(buffer.empty());
}



// Test the instantiation of a non-predefined instantiation type.
BOOST_AUTO_TEST_CASE(Noninstantiated)
{
    constexpr size_t TESTSIZE = 8u;
    struct Example {
        int x;
        float y;
    };
    oscpp::CircularBuffer<Example> buffer{8};

    for (int n = 0; n < static_cast<int>(TESTSIZE) - 1; ++n) {
        Example s = {.x= n, .y= 2.0f*n};
        buffer.put(s);
    }
    BOOST_CHECK(buffer.full());
}

BOOST_AUTO_TEST_SUITE_END()
