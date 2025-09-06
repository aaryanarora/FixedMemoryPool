#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>

template <typename T>
class MemoryPool {
public:
    // Constructor: allocates memory pool and initializes free offset array
    explicit MemoryPool(size_t poolSize);
    
    // Destructor: deallocates the memory pool
    ~MemoryPool();
    
    // Allocate memory for an object of type T
    T* allocate();
    
    // Free memory and return it to the pool
    void deallocate(T* ptr);

    
    // Get the total pool size
    size_t getPoolSize() const { return poolSize; }
    
    // Get the number of available slots
    size_t getAvailableSlots() const { return availableSlots; }
    
    // Check if pool is full
    bool isFull() const { return availableSlots == 0; }
    
    // Check if pool is empty
    bool isEmpty() const { return availableSlots == poolSize; }

private:
    char* memoryPool;           // Raw memory pool
    size_t* freeOffsets;        // Array of free offsets
    size_t poolSize;            // Total number of objects in pool
    size_t availableSlots;      // Number of available slots
    size_t nextFreeIndex;       // Index of next free slot in freeOffsets array
    
    // Size of each object (aligned)
    static constexpr size_t objectSize = sizeof(T);
    
};

#include "MemoryPool.hpp"



