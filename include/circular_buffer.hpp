// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#ifndef OSCPP_CIRCULAR_BUFFER_HPP
#define OSCPP_CIRCULAR_BUFFER_HPP


#include <mutex>
#include <condition_variable>

using std::unique_lock;
using std::mutex;


/**
 * Circular Buffer, a double-ended queue that uses a static array and reuses
 * memory.
 *
 * @author Glen Dayton
 */
namespace oscpp {
    template<typename ElementType>
    class CircularBuffer {
    public:
        /**
         * Minimum size of the buffer must include space for the wrap-around.
         */
        static constexpr size_t MinSize = 4u;
        /**
         * Create an empty circular buffer at least 'nSize' big.
         * The actual size of the buffer will be rounded up to a power of 2.
         *
         * @param nSize Minimum size of the buffer
         */
        explicit CircularBuffer(size_t nSize);

        /// Deallocate the buffer
        ~CircularBuffer();

        CircularBuffer(const CircularBuffer &) = delete;

        CircularBuffer &operator=(const CircularBuffer &) = delete;

        /// Return the capacity of the buffer
        [[nodiscard]] auto capacity() const -> size_t;

        /// Predicate for testing whether buffer is empty
        [[nodiscard]] auto empty() const -> bool;

        /// Predicate for testing whether the buffer is full
        [[nodiscard]] auto full() const -> bool;

        /**
         * Get a value from the buffer.  Blocks until it is done.
         * @return ElementType
         */
        auto get() -> ElementType;

        /// returns the number of elements in the buffer
        [[nodiscard]] auto size() const -> size_t;

        /**
         * Try getting a value from the buffer
         * @return bool true on success
         */
        auto tryGet(ElementType &value) -> bool;

        /**
         * Try putting an element into the buffer
         * @return bool true if successful
         */
        auto tryPut(ElementType value) -> bool;

        /**
         * Put a value into the buffer.  Blocks until it is done.
         * @param value Value to insert into buffer
         */
        auto put(ElementType value) -> void;

    protected:
        // Utility for rounding up the size of the buffer to a power of 2 to
        // simplify modulo operation
        static auto roundup(size_t n) -> size_t;

        // Utility for getting the index of the next element in the buffer,
        // modulo the size of the buffer.
        [[nodiscard]] auto next(size_t i) const -> size_t;

    private:
        // Non-locking versions of empty()/full() for use by callers that
        // already hold `guard`. head and tail must never be read or written
        // without holding `guard`.
        [[nodiscard]] auto emptyLocked() const -> bool;
        [[nodiscard]] auto fullLocked() const -> bool;

        const size_t bufferCapacity;
        const size_t bufferCapacityMinus1;
        ElementType *const buffer;
        size_t head;
        size_t tail;

        // Protects head and tail, and is held while waiting on notEmpty/notFull.
        // mutable so const methods (empty(), full(), size()) can lock it.
        mutable std::mutex guard;
        std::condition_variable notEmpty;
        std::condition_variable notFull;
    };
}
#endif // OSCPP_CIRCULAR_BUFFER_HPP
