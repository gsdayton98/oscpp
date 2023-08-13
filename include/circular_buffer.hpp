#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <cstdlib>
#include <cstdint>
#include <mutex>
#include <condition_variable>

using std::unique_lock;
using std::mutex;


/**
 * Circular Buffer, a double ended queue that uses a static array and reuses
 * memory.
 *
 * @author Glen Dayton
 */
template<typename ElementType>
class CircularBuffer {
public:
    /**
     * Create an empty circular buffer at least 'nSize' big.
     * Actual size of the buffer will be rounded up to a power of 2.
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
    [[nodiscard]] auto empty() const-> bool;

    /// Predicate for testing whether buffer is full
    [[nodiscard]] auto full() -> bool;

    /**
     * Get a value from the buffer.  Blocks until it is done.
     * @return ElementType
     */
    auto get() -> ElementType;

    /// returns number of elements in the buffer
    [[nodiscard]] auto size() -> size_t;

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
    [[nodiscard]] auto next(size_t i) -> size_t;

private:
    const size_t bufferCapacity;
    const size_t bufferCapacityMinus1;
    ElementType *const buffer;
    volatile size_t head;
    volatile size_t tail;

    // Protects head and tail
    std::mutex readMutex;
    std::condition_variable notEmpty;
    std::mutex writeMutex;
    std::condition_variable notFull;
};

#endif // CIRCULAR_BUFFER_HPP
