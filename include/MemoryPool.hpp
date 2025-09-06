template <typename T>
MemoryPool<T>::MemoryPool(size_t poolSize) 
    : poolSize(poolSize), availableSlots(poolSize), nextFreeIndex(0) {
    
    if (poolSize == 0) {
        throw std::invalid_argument("Pool size must be greater than 0");
    }
    
    // Allocate raw memory pool
    memoryPool =  static_cast<char*>(std::malloc(poolSize * objectSize));
    
    // Allocate free offsets array
    freeOffsets = static_cast<size_t*>(std::malloc(poolSize * sizeof(size_t)));
    
    // Initialize free offsets array with sequential offsets
    for (size_t i = 0; i < poolSize; ++i) {
        freeOffsets[i] = i;
    }
}

template <typename T>
MemoryPool<T>::~MemoryPool() {
    std::free(memoryPool);
    std::free(freeOffsets);
}

template <typename T>
T* MemoryPool<T>::allocate() {
    if (availableSlots == 0) {
        std::cerr<<"Pool is full, change the size of the pool"<<std::endl;
        return nullptr; // Pool is full
    }
    
    // Get the next free offset
    size_t offset = freeOffsets[nextFreeIndex];
    
    // Move to next free slot
    nextFreeIndex++;
    availableSlots--;
    
    // Return pointer to allocated memory
    return reinterpret_cast<T*>(memoryPool + offset);
}

template <typename T>
void MemoryPool<T>::deallocate(T* ptr) {
    if (ptr == nullptr) {
        std::cerr<<"Pointer is nullptr, cannot deallocate"<<std::endl;
        return;
    }
    
    // Calculate offset of the pointer
    size_t offset = (static_cast<char*>(static_cast<void*>(ptr)) - memoryPool) / objectSize;
    
    // Validate that the pointer belongs to this pool
    if (offset >= poolSize) {
        throw std::invalid_argument("Pointer does not belong to this memory pool");
    }
    
    nextFreeIndex--;
    availableSlots++;
    freeOffsets[nextFreeIndex] = offset;
}

