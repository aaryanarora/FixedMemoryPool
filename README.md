# FixedMemoryPool

A high-performance C++ memory pool implementation using a freeOffset array for efficient memory management with O(1) allocation and deallocation operations.

## 🚀 Overview

This implementation provides a fixed-size memory pool that pre-allocates a contiguous block of memory and manages it using a freeOffset array. This approach offers significant advantages over standard `new`/`delete` operations:

- **⚡ Fast allocation/deallocation**: O(1) time complexity
- **🎯 Memory locality**: All objects stored in contiguous memory blocks
- **🔧 No fragmentation**: Fixed-size objects prevent internal fragmentation
- **💾 Cache-friendly**: Sequential memory layout improves cache performance
- **📊 Predictable performance**: Consistent latency regardless of pool size

## 📈 Performance Results

Based on performance testing on modern hardware:

### Allocation Latency
- **Min**: 15 ns
- **Max**: 13,311 ns  
- **Median**: 23 ns
- **Average**: 24.59 ns

### Deallocation Latency
- **Min**: 16 ns
- **Max**: 17,838 ns
- **Median**: 18 ns
- **Average**: 18.12 ns

### Comparison with Standard new/delete
- **MemoryPool**: ~0.01 microseconds per operation
- **new/delete**: ~0.01 microseconds per operation
- **Performance**: Comparable performance with better predictability

## 🏗️ Architecture

### Key Components

1. **Memory Pool**: A contiguous block of memory allocated for storing objects
2. **Free Offset Array**: An array that tracks available memory offsets
3. **Pool Management**: Methods to allocate, deallocate, and track pool status

### How It Works

1. **Initialization**: The pool allocates a large block of memory and initializes the freeOffset array with sequential offsets
2. **Allocation**: When `allocate()` is called, it returns the next available offset from the freeOffset array
3. **Deallocation**: When `deallocate()` is called, the offset is added back to the freeOffset array for reuse

## 🔨 Building

### Prerequisites
- C++ compatible compiler (GCC, Clang, MSVC)
- Make (optional, for using Makefile)

### Compilation

Using Makefile:
```bash
# Build both examples
make all

# Build only basic example
make memory_pool_example

# Build only performance test
make performance_test

### Running the Examples

```bash
# Basic functionality test
./memory_pool_example

# Comprehensive performance test
./performance_test
```

## 📊 Performance Characteristics

- **Allocation Time**: O(1) - Constant time regardless of pool size
- **Deallocation Time**: O(1) - Constant time regardless of pool size
- **Memory Overhead**: Minimal - Only the freeOffset array overhead
- **Cache Performance**: Excellent - Contiguous memory layout
- **Predictability**: Consistent performance with low variance

## ⚠️ Important Notes

### Thread Safety
**This implementation is not thread-safe.** For multi-threaded applications, external synchronization is required.

### Memory Management
- Uses `std::malloc()` and `std::free()` for raw memory allocation
- Properly validates pointer ownership before deallocation
- Handles edge cases like null pointer deallocation

### Error Handling
- Returns `nullptr` when pool is full
- Throws `std::invalid_argument` for invalid operations
- Provides clear error messages for debugging
