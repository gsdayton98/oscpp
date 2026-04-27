//
// Created by Glen Dayton on 8/11/23.
//
#include "circular_buffer.hpp"

template<typename ElementType>
oscpp::CircularBuffer<ElementType>::CircularBuffer(const size_t nSize)
            : bufferCapacity{roundup(nSize)},
              bufferCapacityMinus1{bufferCapacity - 1UL},
              buffer{new ElementType[bufferCapacity]},
              head{0UL},
              tail{0UL}
    {}

    /// Deallocate the buffer
    template<typename ElementType>
    oscpp::CircularBuffer<ElementType>::~CircularBuffer() {
        delete[] buffer;
    }

    /// Return the capacity of the buffer
    template<typename ElementType>
    [[nodiscard]] auto oscpp::CircularBuffer<ElementType>::capacity() const -> size_t { return bufferCapacity; }

    /// Predicate for testing whether buffer is empty
    template<typename ElementType>
    [[nodiscard]] auto oscpp::CircularBuffer<ElementType>::empty() const-> bool { return head == tail; }

    /// Predicate for testing whether the buffer is full
    template<typename ElementType>
    [[nodiscard]] auto oscpp::CircularBuffer<ElementType>::full() const -> bool { return next(head) == tail; }

    /**
     * Get a value from the buffer.  Blocks until it is done.
     * @return ElementType
     */
    template<typename ElementType>
    auto oscpp::CircularBuffer<ElementType>::get() -> ElementType {
        // Lock the read mutex to reserve the current tail value,
        std::unique_lock lock(readMutex);
        notEmpty.wait(lock, [this]{ return ! empty(); });
        auto val = buffer[tail];
        tail = next(tail);
        lock.unlock();

        notFull.notify_all();
        return val;
    }

    /// returns the number of elements in the buffer
    template<typename ElementType>
    [[nodiscard]] auto oscpp::CircularBuffer<ElementType>::size() const -> size_t {
        return head + bufferCapacity - tail & bufferCapacityMinus1;
    }

    /**
     * Try getting a value from the buffer
     * @return bool true on success
     */
    template<typename ElementType>
    auto oscpp::CircularBuffer<ElementType>::tryGet(ElementType &value) -> bool {
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
auto oscpp::CircularBuffer<ElementType>::tryPut(ElementType value) -> bool {
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
auto oscpp::CircularBuffer<ElementType>::put(ElementType value) -> void {
    unique_lock lock(writeMutex);
    notFull.wait(lock, [this]{ return ! full();});
    buffer[head] = value;
    head = next(head);
    lock.unlock();
    notEmpty.notify_all();
}

// Utility for rounding up the size of the buffer to a power of 2 to
// simplify modulo operation
template<typename ElementType>
auto oscpp::CircularBuffer<ElementType>::roundup(const size_t n) -> size_t {
    unsigned int k = 1;
    while (k < n) {
        k <<= 1;
    }
    return k;
}

// Utility for getting the index of the next element in the buffer,
// modulo the size of the buffer.
template<typename ElementType>
[[nodiscard]] auto oscpp::CircularBuffer<ElementType>::next(const size_t i) const -> size_t {
    return i + 1 & bufferCapacityMinus1;
}

template class __attribute__((visibility("default"))) oscpp::CircularBuffer<bool>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned char>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned short>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned int>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<unsigned long long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<char>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<short>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<int>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long long>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<float>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<double>;
template class __attribute__((visibility("default"))) oscpp::CircularBuffer<long double>;
