// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
#define BOOST_BOOST_AUTO_TEST_MODULE Test CircularBuffer
#include <boost/test/unit_test.hpp>

#include <atomic>
#include <thread>
#include <vector>
#include "circular_buffer.hpp"

template
class oscpp::CircularBuffer<uintptr_t>;

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


    // Begin Claude AI Generated Code
    // Packs a producer id and that producer's per-value sequence number into
    // a single uintptr_t so the multithreaded tests can verify, after the
    // fact, that every produced value was received exactly once and intact.
    // EncodeShift is small enough that the encoding is lossless even when
    // uintptr_t is only 32 bits wide.
    constexpr unsigned EncodeShift = 20u;
    constexpr size_t MaxSeqPerProducer = 1u << EncodeShift;

    constexpr auto encode(const size_t producerId, const size_t seq) -> uintptr_t {
        return (static_cast<uintptr_t>(producerId) << EncodeShift) | static_cast<uintptr_t>(seq);
    }

    constexpr auto decodeProducer(const uintptr_t value) -> size_t { return value >> EncodeShift; }

    constexpr auto decodeSeq(const uintptr_t value) -> size_t { return value & (MaxSeqPerProducer - 1u); }

    // Confirms that 'results' contains, in any order, exactly one value for
    // every (producerId, seq) pair produced -- i.e. nothing was dropped,
    // duplicated, or corrupted in transit through the buffer.
    void verifyAllValuesReceivedExactlyOnce(const std::vector<uintptr_t> &results,
                                             const size_t numProducers,
                                             const size_t itemsPerProducer) {
        std::vector seen(numProducers, std::vector(itemsPerProducer, false));
        for (const uintptr_t value : results) {
            const size_t producerId = decodeProducer(value);
            const size_t seq = decodeSeq(value);
            BOOST_REQUIRE_LT(producerId, numProducers);
            BOOST_REQUIRE_LT(seq, itemsPerProducer);
            BOOST_REQUIRE_MESSAGE(!seen[producerId][seq],
                                   "duplicate value received: producer " << producerId << " seq " << seq);
            seen[producerId][seq] = true;
        }

        for (size_t p = 0; p < numProducers; ++p) {
            for (size_t seq = 0; seq < itemsPerProducer; ++seq) {
                BOOST_REQUIRE_MESSAGE(seen[p][seq],
                                       "missing value: producer " << p << " seq " << seq);
            }
        }
    }
    // End Claude AI Generated Code
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

    for (auto n=0; n < TESTSIZE-1; ++n) {
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

// Begin Claude AI Generated Code
// Multiple producer threads call the blocking put(), multiple consumer
// threads call the blocking get(), against a buffer far smaller than the
// total number of items transferred, so every put()/get() pair is forced to
// block and hand off across threads many times. Each consumer claims its
// slot in the results array via an atomic counter before calling get(), so
// there is no locking beyond the buffer's own -- any data race in
// put()/get() would show up as a dropped, duplicated, or corrupted value.
BOOST_AUTO_TEST_CASE(MultithreadedBlockingPutGet) {
    constexpr size_t numProducers = 4;
    constexpr size_t numConsumers = 4;
    constexpr size_t itemsPerProducer = 20000;
    constexpr size_t totalItems = numProducers * itemsPerProducer;
    static_assert(itemsPerProducer <= MaxSeqPerProducer);

    oscpp::CircularBuffer<uintptr_t> buffer{16};

    std::vector<uintptr_t> results(totalItems);
    std::atomic<size_t> claimIndex{0};

    std::vector<std::thread> producers;
    producers.reserve(numProducers);
    for (size_t p = 0; p < numProducers; ++p) {
        producers.emplace_back([&buffer, p] {
            for (size_t seq = 0; seq < itemsPerProducer; ++seq) {
                buffer.put(encode(p, seq));
            }
        });
    }

    std::vector<std::thread> consumers;
    consumers.reserve(numConsumers);
    for (size_t c = 0; c < numConsumers; ++c) {
        consumers.emplace_back([&buffer, &results, &claimIndex] {
            for (;;) {
                const size_t index = claimIndex.fetch_add(1, std::memory_order_relaxed);
                if (index >= totalItems) {
                    break;
                }
                results[index] = buffer.get();
            }
        });
    }

    for (auto &t: producers) { t.join(); }
    for (auto &t: consumers) { t.join(); }

    BOOST_CHECK(buffer.empty());
    verifyAllValuesReceivedExactlyOnce(results, numProducers, itemsPerProducer);
}

// Same producer/consumer shape as MultithreadedBlockingPutGet, but using the
// non-blocking tryPut()/tryGet() in busy-retry loops against a buffer at
// CircularBuffer::MinSize. This maximizes the rate of full<->empty
// transitions and head/tail wraparound, stressing the check-then-act path
// in tryPut()/tryGet() specifically.
BOOST_AUTO_TEST_CASE(MultithreadedTryPutTryGet) {
    constexpr size_t numProducers = 6;
    constexpr size_t numConsumers = 6;
    constexpr size_t itemsPerProducer = 3000;
    constexpr size_t totalItems = numProducers * itemsPerProducer;
    static_assert(itemsPerProducer <= MaxSeqPerProducer);

    oscpp::CircularBuffer<uintptr_t> buffer{oscpp::CircularBuffer<uintptr_t>::MinSize};

    std::vector<uintptr_t> results(totalItems);
    std::atomic<size_t> claimIndex{0};
    std::atomic<size_t> producedCount{0};

    std::vector<std::thread> producers;
    producers.reserve(numProducers);
    for (size_t p = 0; p < numProducers; ++p) {
        producers.emplace_back([&buffer, &producedCount, p] {
            for (size_t seq = 0; seq < itemsPerProducer; ++seq) {
                while (!buffer.tryPut(encode(p, seq))) {
                    std::this_thread::yield();
                }
                producedCount.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    std::vector<std::thread> consumers;
    consumers.reserve(numConsumers);
    for (size_t c = 0; c < numConsumers; ++c) {
        consumers.emplace_back([&buffer, &results, &claimIndex] {
            for (;;) {
                const size_t index = claimIndex.fetch_add(1, std::memory_order_relaxed);
                if (index >= totalItems) {
                    break;
                }
                uintptr_t value;
                while (!buffer.tryGet(value)) {
                    std::this_thread::yield();
                }
                results[index] = value;
            }
        });
    }

    for (auto &t: producers) { t.join(); }
    for (auto &t: consumers) { t.join(); }

    BOOST_CHECK_EQUAL(totalItems, producedCount.load());
    BOOST_CHECK(buffer.empty());
    verifyAllValuesReceivedExactlyOnce(results, numProducers, itemsPerProducer);
}
// End Claude AI generated code
// End Claude AI generated code


// Test the instantiation of a non-predefined instantiation type.
BOOST_AUTO_TEST_CASE(Noninstantiated)
{
    constexpr size_t TESTSIZE = 8u;
    struct Example {
        int x;
        float y;
    };
    oscpp::CircularBuffer<Example> buffer{8};

    for (int n=0; n < TESTSIZE - 1; ++n) {
        Example s = {.x= n, .y= 2.0f*n};
        buffer.put(s);
    }
    BOOST_CHECK(buffer.full());
}

BOOST_AUTO_TEST_SUITE_END()