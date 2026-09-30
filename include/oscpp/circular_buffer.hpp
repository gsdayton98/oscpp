// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2023 Glen S. Dayton. Rights reserved according to terms of included license.

#ifndef OSCPP_CIRCULAR_BUFFER_HPP
#define OSCPP_CIRCULAR_BUFFER_HPP


#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <mutex>

/**
 * Circular Buffer: a thread-safe, fixed-capacity FIFO queue that reuses its memory. Storage is a heap-allocated array
 * whose size is rounded up to a power of two. put() and get() block while the buffer is full or empty; tryPut() and
 * tryGet() don't.
 *
 * This is a header-only template. ElementType must be default-constructible and copy-assignable.
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
        static constexpr std::size_t MinSize = 4u;
        /**
         * Create an empty circular buffer at least 'nSize' big.
         * The actual size of the buffer will be rounded up to a power of 2.
         *
         * @param nSize Minimum size of the buffer
         */
        explicit CircularBuffer(std::size_t nSize);

        /// Deallocate the buffer
        ~CircularBuffer();

        CircularBuffer(const CircularBuffer &) = delete;

        CircularBuffer &operator=(const CircularBuffer &) = delete;

        /// Return the capacity of the buffer
        [[nodiscard]] auto capacity() const -> std::size_t;

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
        [[nodiscard]] auto size() const -> std::size_t;

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
        static auto roundup(std::size_t n) -> std::size_t;

        // Utility for getting the index of the next element in the buffer,
        // modulo the size of the buffer.
        [[nodiscard]] auto next(std::size_t i) const -> std::size_t;

    private:
        // Non-locking versions of empty()/full() for use by callers that
        // already hold `guard`. head and tail must never be read or written
        // without holding `guard`.
        [[nodiscard]] auto emptyLocked() const -> bool;
        [[nodiscard]] auto fullLocked() const -> bool;

        const std::size_t bufferCapacity;
        const std::size_t bufferCapacityMinus1;
        ElementType *const buffer;
        std::size_t head;
        std::size_t tail;

        // Protects head and tail, and is held while waiting on notEmpty/notFull.
        // mutable so const methods (empty(), full(), size()) can lock it.
        mutable std::mutex guard;
        std::condition_variable notEmpty;
        std::condition_variable notFull;
    };
}

namespace oscpp {

    template<typename ElementType>
    CircularBuffer<ElementType>::CircularBuffer(const std::size_t nSize)
        : bufferCapacity{roundup(nSize)},
          bufferCapacityMinus1{bufferCapacity - 1UL},
          buffer{new ElementType[bufferCapacity]},
          head{0UL},
          tail{0UL}
    {}

    /// Deallocate the buffer
    template<typename ElementType>
    CircularBuffer<ElementType>::~CircularBuffer() {
        delete[] buffer;
    }

    /// Return the capacity of the buffer.
    /// Returned capacity is one less that the actual space available because one space separates the write and
    /// read pointers.
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::capacity() const -> std::size_t { return bufferCapacity - 1; }

    /// Predicate for testing whether buffer is empty
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::empty() const -> bool {
        std::unique_lock lock(guard);
        return emptyLocked();
    }

    /// Predicate for testing whether the buffer is full
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::full() const -> bool {
        std::unique_lock lock(guard);
        return fullLocked();
    }

    // Non-locking versions of empty()/full(); caller must hold `guard`.
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::emptyLocked() const -> bool { return head == tail; }

    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::fullLocked() const -> bool { return next(head) == tail; }

    /**
     * Get a value from the buffer.  Blocks until it is done.
     * @return ElementType
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::get() -> ElementType {
        std::unique_lock lock(guard);
        notEmpty.wait(lock, [this]{ return ! emptyLocked(); });
        auto val = buffer[tail];
        tail = next(tail);
        lock.unlock();

        notFull.notify_all();
        return val;
    }

    /// returns the number of elements in the buffer
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::size() const -> std::size_t {
        std::unique_lock lock(guard);
        return ((head + bufferCapacity) - tail) & bufferCapacityMinus1;
    }

    /**
     * Try getting a value from the buffer
     * @return bool true on success
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::tryGet(ElementType &value) -> bool {
        std::unique_lock lock(guard);
        bool status = false;
        if (!emptyLocked()) {
            value = buffer[tail];
            tail = next(tail);
            status = true;
        }
        lock.unlock();
        if (status) {
            notFull.notify_all();
        }
        return status;
    }

    /**
     * Try putting an element into the buffer
     * @return bool true if successful
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::tryPut(ElementType value) -> bool {
        std::unique_lock lock(guard);
        bool status = false;
        if (!fullLocked()) {
            buffer[head] = value;
            head = next(head);
            status = true;
        }
        lock.unlock();
        if (status) {
            notEmpty.notify_all();
        }
        return status;
    }

    /**
     * Put a value into the buffer.  Blocks until it is done.
     * @param value Value to insert into buffer
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::put(ElementType value) -> void {
        std::unique_lock lock(guard);
        notFull.wait(lock, [this]{ return ! fullLocked();});
        buffer[head] = value;
        head = next(head);
        lock.unlock();
        notEmpty.notify_all();
    }

    // Utility for rounding up the size of the buffer to a power of 2 to
    // simplify modulo operation
    template<typename ElementType>
    auto CircularBuffer<ElementType>::roundup(const std::size_t n) -> std::size_t {
        std::size_t k = 1;
        auto nn = std::max(CircularBuffer::MinSize, n);
        while (k < nn) {
            k <<= 1;
        }
        return k;
    }

    // Utility for getting the index of the next element in the buffer,
    // modulo the size of the buffer.
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::next(const std::size_t i) const -> std::size_t {
        return (i + 1) & bufferCapacityMinus1;
    }
}

#endif // OSCPP_CIRCULAR_BUFFER_HPP
