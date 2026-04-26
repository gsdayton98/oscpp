// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
#include <chrono>
#include "CppUnitXLite/CppUnitXLite.cpp"
#include <thread>
#include "circular_buffer.hpp"

template
class oscpp::CircularBuffer<uintptr_t>;


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


TEST(TestCircularBuffer, Roundup) {
    TestCase testCases[] = {
            TestCase{0, 1},
            TestCase{1, 1},
            TestCase{2, 2},
            TestCase{3, 4},
            TestCase{4, 4},
            TestCase{5, 8},
            TestCase{7, 8},
            TestCase{8, 8},
            TestCase{60, 64}
    };

    for (const TestCase &aTestCase: testCases) {
        CHECK_EQUAL(aTestCase.expected, TesterCircularBuffer::roundup(aTestCase.arg));
    }
}

TEST(TestCircularBuffer, Construction) {
    const oscpp::CircularBuffer<uintptr_t> buffer{7};

    CHECK_EQUAL(8UL, buffer.capacity());
}

TEST(TestCircularBuffer, Next) {
    const TesterCircularBuffer buffer{8};

    for (size_t i = 0; i < 2 * buffer.capacity(); ++i) {
        const size_t j = buffer.next(i);
        CHECK_EQUAL((i + 1UL) % buffer.capacity(), j);
    }
}

TEST(TestCircularBuffer, Zero) {
    const oscpp::CircularBuffer<uintptr_t> buffer{0};

    CHECK(buffer.empty());
    CHECK(buffer.full());
}

TEST(TestCircularBuffer, Size) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        CHECK(buffer.tryPut(x));
    }
    CHECK_EQUAL(5UL, buffer.size());
}

TEST(TestCircularBuffer, ReadWrite1) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        CHECK(buffer.tryPut(x));
    }


    uintptr_t justRead;
    for (uintptr_t x = 1UL; x <= 5UL; ++x) {
        CHECK(buffer.tryGet(justRead));
        CHECK_EQUAL(x, justRead);
    }
}

TEST(TestCircularBuffer, ReadWriteX) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    uintptr_t expectedRead = 1UL;
    uintptr_t justRead;
    for (uintptr_t x = 1UL; x <= 25UL; ++x) {
        while (!buffer.tryPut(x)) {
            CHECK(buffer.tryGet(justRead));
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            CHECK(buffer.tryGet(justRead));
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
            CHECK(buffer.tryGet(justRead));
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
        }
    }

    while (buffer.tryGet(justRead)) {
        CHECK_EQUAL(expectedRead, justRead);
        ++expectedRead;
    }
    CHECK(buffer.empty());
}


TEST(TestCircularBuffer, Singlethread) {
    oscpp::CircularBuffer<uintptr_t> buffer{8};

    uintptr_t expectedRead = 1UL;
    uintptr_t justRead;

    for (uintptr_t x = 1UL; x <= 25UL; ++x) {
        buffer.put(x);
        while (buffer.full()) {
            justRead = buffer.get();
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            justRead = buffer.get();
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;

            justRead = buffer.get();
            CHECK_EQUAL(expectedRead, justRead);
            ++expectedRead;
        }
    }

    while (!buffer.empty()) {
        justRead = buffer.get();
        CHECK_EQUAL(expectedRead, justRead);
        ++expectedRead;
    }
    CHECK(buffer.empty());
}

using std::chrono::duration;
using std::chrono::milliseconds;

[[maybe_unused]]
struct TestCircularBufferMultiThreadTest : Test {
    oscpp::CircularBuffer<uintptr_t> buffer;
    static constexpr uintptr_t ITERATIONS = 50UL;

    TestCircularBufferMultiThreadTest()
            : Test("CppUnitXLiteTest::TestCircularBufferMultiThreadTest"),
              buffer{8} {}


    // CppUnitXLite is single threaded, so only one thread may post results
    void testThread(TestResult &result) {
        constexpr auto PERIOD = milliseconds(30);

        for (uintptr_t expected = 1UL; expected <= ITERATIONS; ++expected) {
            std::this_thread::sleep_for(PERIOD);
            const uintptr_t actual = buffer.get();

            checkEqual(expected, actual, result, __FILE__, __LINE__);
        }
    }

    void otherThread() {
        constexpr auto PERIOD = milliseconds(55);

        for (uintptr_t actual = 1UL; actual <= ITERATIONS; ++actual) {
            buffer.put(actual);
            std::this_thread::sleep_for(PERIOD);
        }
    }

    void run(TestResult &result) override {
        std::thread consumer(&TestCircularBufferMultiThreadTest::testThread, this, std::ref(result));
        std::thread producer(&TestCircularBufferMultiThreadTest::otherThread, this);
        consumer.join();
        producer.join();
    }

} TestCircularBufferMultiThreadTestInstance;


TESTMAIN

