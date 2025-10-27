#ifndef _Kernel_RingBuffer_h_
#define _Kernel_RingBuffer_h_

#include "Common.h"
#include "Defs.h"

// Simple ring buffer implementation
template<typename T, uint32 BUFFER_SIZE>
class RingBuffer {
private:
    T buffer[BUFFER_SIZE];
    uint32 head;
    uint32 tail;
    uint32 count;
    Spinlock lock;

public:
    RingBuffer() : head(0), tail(0), count(0) {
        // Initialize the spinlock
        lock.Initialize();
    }

    ~RingBuffer() {
        // Destructor
    }

    // Add an element to the ring buffer
    bool Push(const T& item) {
        // Acquire lock
        lock.Acquire();

        // Check if buffer is full
        if (count >= BUFFER_SIZE) {
            // Release lock
            lock.Release();
            return false;
        }

        // Add item to buffer
        buffer[tail] = item;
        tail = (tail + 1) % BUFFER_SIZE;
        count++;

        // Release lock
        lock.Release();
        return true;
    }

    // Remove an element from the ring buffer
    bool Pop(T& item) {
        // Acquire lock
        lock.Acquire();

        // Check if buffer is empty
        if (count == 0) {
            // Release lock
            lock.Release();
            return false;
        }

        // Remove item from buffer
        item = buffer[head];
        head = (head + 1) % BUFFER_SIZE;
        count--;

        // Release lock
        lock.Release();
        return true;
    }

    // Peek at the front element without removing it
    bool Peek(T& item) {
        // Acquire lock
        lock.Acquire();

        // Check if buffer is empty
        if (count == 0) {
            // Release lock
            lock.Release();
            return false;
        }

        // Peek at front item
        item = buffer[head];

        // Release lock
        lock.Release();
        return true;
    }

    // Get the number of elements in the buffer
    uint32 GetCount() {
        // Acquire lock
        lock.Acquire();

        // Get count
        uint32 result = count;

        // Release lock
        lock.Release();
        return result;
    }

    // Check if the buffer is empty
    bool IsEmpty() {
        return (GetCount() == 0);
    }

    // Check if the buffer is full
    bool IsFull() {
        return (GetCount() >= BUFFER_SIZE);
    }

    // Clear the buffer
    void Clear() {
        // Acquire lock
        lock.Acquire();

        // Reset pointers
        head = 0;
        tail = 0;
        count = 0;

        // Release lock
        lock.Release();
    }
};

#endif