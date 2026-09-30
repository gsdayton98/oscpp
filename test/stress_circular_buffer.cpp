// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.
//
// Multithreaded stress tests for CircularBuffer. These take too long to be unit tests, so they build into their own
// executable (stress_circular_buffer) and are registered with CTest under the "stress" label.
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <atomic>
#include <thread>
#include <vector>
#include "oscpp/circular_buffer.hpp"

BOOST_AUTO_TEST_SUITE(CircularBufferStress)

namespace {
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
    // Only failures are reported (BOOST_FAIL): per-item BOOST_REQUIRE_* calls log every passing check at verbose log
    // levels, which made this verification take minutes.
    void verifyAllValuesReceivedExactlyOnce(const std::vector<uintptr_t> &results,
                                             const size_t numProducers,
                                             const size_t itemsPerProducer) {
        std::vector seen(numProducers, std::vector(itemsPerProducer, false));
        for (const uintptr_t value : results) {
            const size_t producerId = decodeProducer(value);
            const size_t seq = decodeSeq(value);
            if (producerId >= numProducers || seq >= itemsPerProducer) {
                BOOST_FAIL("corrupted value received: producer " << producerId << " seq " << seq);
            }
            if (seen[producerId][seq]) {
                BOOST_FAIL("duplicate value received: producer " << producerId << " seq " << seq);
            }
            seen[producerId][seq] = true;
        }

        for (size_t p = 0; p < numProducers; ++p) {
            for (size_t seq = 0; seq < itemsPerProducer; ++seq) {
                if (!seen[p][seq]) {
                    BOOST_FAIL("missing value: producer " << p << " seq " << seq);
                }
            }
        }
    }
    // End Claude AI Generated Code
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

BOOST_AUTO_TEST_SUITE_END()
