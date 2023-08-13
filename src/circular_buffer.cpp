//
// Created by Glen Dayton on 8/11/23.
//
#include "circular_buffer.hpp"

template<typename ElementType>
CircularBuffer<ElementType>::CircularBuffer(size_t nSize)
            : bufferCapacity{CircularBuffer::roundup(nSize)},
              buffer{new ElementType[bufferCapacity]},
              bufferCapacityMinus1{bufferCapacity - 1UL},
              head{0UL},
              tail{0UL},
              readMutex{},
              writeMutex{},
              notEmpty{},
              notFull{}
    {}

    /// Deallocate the buffer
    template<typename ElementType>
    CircularBuffer<ElementType>::~CircularBuffer() {
        delete[] buffer;
    }

    /// Return the capacity of the buffer
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::capacity() const -> size_t { return bufferCapacity; }

    /// Predicate for testing whether buffer is empty
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::empty() const-> bool { return head == tail; }

    /// Predicate for testing whether buffer is full
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::full() -> bool { return next(head) == tail; }

    /**
     * Get a value from the buffer.  Blocks until it is done.
     * @return ElementType
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::get() -> ElementType {
        // Lock the read mutex to reserve the current tail value,
        std::unique_lock lock(readMutex);
        notEmpty.wait(lock, [this]{ return ! empty(); });
        auto val = buffer[tail];
        tail = next(tail);
        lock.unlock();

        notFull.notify_all();
        return val;
    }

    /// returns number of elements in the buffer
    template<typename ElementType>
    [[nodiscard]] auto CircularBuffer<ElementType>::size() -> size_t {
        return (head + bufferCapacity - tail) & bufferCapacityMinus1;
    }

    /**
     * Try getting a value from the buffer
     * @return bool true on success
     */
    template<typename ElementType>
    auto CircularBuffer<ElementType>::tryGet(ElementType &value) -> bool {
        bool status = false;
        if (!empty()) {
            std::unique_lock lock(readMutex);
            value = buffer[tail];
            tail = next(tail);
            status = true;
        }
        notFull.notify_all();
        return status;
}

/**
 * Try putting an element into the buffer
 * @return bool true if successful
 */
template<typename ElementType>
auto CircularBuffer<ElementType>::tryPut(ElementType value) -> bool {
    bool status = false;
    if (!full()) {
        std::unique_lock lock(writeMutex);
        buffer[head] = value;
        head = next(head);
        status = true;
    }
    notEmpty.notify_all();
    return status;
}

/**
 * Put a value into the buffer.  Blocks until it is done.
 * @param value Value to insert into buffer
 */
template<typename ElementType>
auto CircularBuffer<ElementType>::put(ElementType value) -> void {
    unique_lock<mutex> lock(writeMutex);
    notFull.wait(lock, [this]{ return ! full();});
    buffer[head] = value;
    head = next(head);
    lock.unlock();
    notEmpty.notify_all();
}

// Utility for rounding up the size of the buffer to a power of 2 to
// simplify modulo operation
template<typename ElementType>
auto CircularBuffer<ElementType>::roundup(size_t n) -> size_t {
    unsigned int k = 1;
    while (k < n) {
        k <<= 1;
    }
    return k;
}

// Utility for getting the index of the next element in the buffer,
// modulo the size of the buffer.
template<typename ElementType>
[[nodiscard]] auto CircularBuffer<ElementType>::next(size_t i) -> size_t {
    return (i + 1) & bufferCapacityMinus1;
}

template class CircularBuffer<bool>;
template class CircularBuffer<unsigned char>;
template class CircularBuffer<unsigned short>;
template class CircularBuffer<unsigned int>;
template class CircularBuffer<unsigned long>;
template class CircularBuffer<unsigned long long>;
template class CircularBuffer<char>;
template class CircularBuffer<short>;
template class CircularBuffer<int>;
template class CircularBuffer<long>;
template class CircularBuffer<long long>;
template class CircularBuffer<float>;
template class CircularBuffer<double>;
template class CircularBuffer<long double>;
