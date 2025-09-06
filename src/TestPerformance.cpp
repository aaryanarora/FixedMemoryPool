#include "MemoryPool.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
#include <memory>

// Test object for performance testing
class PerformanceTestObject {
public:
    int id;
    double value;
    char data[64]; // Larger object to test with different sizes
    
    PerformanceTestObject(int i = 0, double v = 0.0) : id(i), value(v) {
        std::fill(data, data + 64, static_cast<char>(i % 256));
    }
};

// Performance measurement utilities
class PerformanceTimer {
public:
    inline void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    inline void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }
    
    inline double getNanoseconds() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    }
    
    inline double getMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    }
    
    inline double getMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
};

// Test allocation latency
void testAllocationLatency() {
    std::cout << "\n=== Allocation Latency Test ===" << std::endl;
    
    const size_t poolSize = 100000;
    const size_t iterations = 1000000;
    
    MemoryPool<PerformanceTestObject> pool(poolSize);
    PerformanceTimer timer;
    std::vector<double> latencies;
    latencies.reserve(iterations);
    
    std::cout << "Testing allocation latency for " << iterations << " allocations..." << std::endl;
    
    for (size_t i = 0; i < iterations; ++i) {
        timer.start();
        PerformanceTestObject* obj = pool.allocate();
        timer.stop();
        
        if (obj) {
            *obj = PerformanceTestObject(static_cast<int>(i), i * 3.14159);
            latencies.push_back(timer.getNanoseconds());
        } else {
            std::cout << "Pool exhausted at iteration " << i << std::endl;
            break;
        }
    }
    
    // Calculate statistics
    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
        double min = latencies.front();
        double max = latencies.back();
        double median = latencies[latencies.size() / 2];
        double avg = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Allocation Latency Statistics (nanoseconds):" << std::endl;
        std::cout << "  Min:    " << min << " ns" << std::endl;
        std::cout << "  Max:    " << max << " ns" << std::endl;
        std::cout << "  Median: " << median << " ns" << std::endl;
        std::cout << "  Average:" << avg << " ns" << std::endl;
        std::cout << "  Total allocations: " << latencies.size() << std::endl;
    }
}

// Test deallocation latency
void testDeallocationLatency() {
    std::cout << "\n=== Deallocation Latency Test ===" << std::endl;
    
    const size_t poolSize = 100000;
    
    MemoryPool<PerformanceTestObject> pool(poolSize);
    std::vector<PerformanceTestObject*> objects;
    objects.reserve(poolSize);
    
    // Pre-allocate objects
    for (size_t i = 0; i < poolSize; ++i) {
        PerformanceTestObject* obj = pool.allocate();
        if (obj) {
            *obj = PerformanceTestObject(static_cast<int>(i), i * 2.71828);
            objects.push_back(obj);
        }
    }
    
    PerformanceTimer timer;
    std::vector<double> latencies;
    latencies.reserve(objects.size());
    
    std::cout << "Testing deallocation latency for " << objects.size() << " deallocations..." << std::endl;
    
    for (auto* obj : objects) {
        timer.start();
        pool.deallocate(obj);
        timer.stop();
        latencies.push_back(timer.getNanoseconds());
    }
    
    // Calculate statistics
    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
        double min = latencies.front();
        double max = latencies.back();
        double median = latencies[latencies.size() / 2];
        double avg = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Deallocation Latency Statistics (nanoseconds):" << std::endl;
        std::cout << "  Min:    " << min << " ns" << std::endl;
        std::cout << "  Max:    " << max << " ns" << std::endl;
        std::cout << "  Median: " << median << " ns" << std::endl;
        std::cout << "  Average:" << avg << " ns" << std::endl;
        std::cout << "  Total deallocations: " << latencies.size() << std::endl;
    }
}

// Test mixed allocation/deallocation patterns
void testMixedOperationsLatency() {
    std::cout << "\n=== Mixed Operations Latency Test ===" << std::endl;
    
    const size_t poolSize = 50000;
    const size_t iterations = 1000000;
    
    MemoryPool<PerformanceTestObject> pool(poolSize);
    std::vector<PerformanceTestObject*> activeObjects;
    PerformanceTimer timer;
    std::vector<double> allocationLatencies;
    std::vector<double> deallocationLatencies;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    std::cout << "Testing mixed operations for " << iterations << " iterations..." << std::endl;
    
    for (size_t i = 0; i < iterations; ++i) {
        // Randomly choose to allocate or deallocate
        if (dis(gen) < 0.6 && !pool.isFull()) { // 60% chance to allocate
            timer.start();
            PerformanceTestObject* obj = pool.allocate();
            timer.stop();
            
            if (obj) {
                *obj = PerformanceTestObject(static_cast<int>(i), i * 1.414);
                activeObjects.push_back(obj);
                allocationLatencies.push_back(timer.getNanoseconds());
            }
        } else if (!activeObjects.empty()) { // Deallocate if we have objects
            std::uniform_int_distribution<> indexDis(0, activeObjects.size() - 1);
            size_t index = indexDis(gen);
            
            timer.start();
            pool.deallocate(activeObjects[index]);
            timer.stop();
            
            deallocationLatencies.push_back(timer.getNanoseconds());
            activeObjects.erase(activeObjects.begin() + index);
        }
    }
    
    // Calculate statistics for allocations
    if (!allocationLatencies.empty()) {
        std::sort(allocationLatencies.begin(), allocationLatencies.end());
        double minAlloc = allocationLatencies.front();
        double maxAlloc = allocationLatencies.back();
        double medianAlloc = allocationLatencies[allocationLatencies.size() / 2];
        double avgAlloc = std::accumulate(allocationLatencies.begin(), allocationLatencies.end(), 0.0) / allocationLatencies.size();
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Mixed Operations - Allocation Latency (nanoseconds):" << std::endl;
        std::cout << "  Min:    " << minAlloc << " ns" << std::endl;
        std::cout << "  Max:    " << maxAlloc << " ns" << std::endl;
        std::cout << "  Median: " << medianAlloc << " ns" << std::endl;
        std::cout << "  Average:" << avgAlloc << " ns" << std::endl;
        std::cout << "  Total allocations: " << allocationLatencies.size() << std::endl;
    }
    
    // Calculate statistics for deallocations
    if (!deallocationLatencies.empty()) {
        std::sort(deallocationLatencies.begin(), deallocationLatencies.end());
        double minDealloc = deallocationLatencies.front();
        double maxDealloc = deallocationLatencies.back();
        double medianDealloc = deallocationLatencies[deallocationLatencies.size() / 2];
        double avgDealloc = std::accumulate(deallocationLatencies.begin(), deallocationLatencies.end(), 0.0) / deallocationLatencies.size();
        
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Mixed Operations - Deallocation Latency (nanoseconds):" << std::endl;
        std::cout << "  Min:    " << minDealloc << " ns" << std::endl;
        std::cout << "  Max:    " << maxDealloc << " ns" << std::endl;
        std::cout << "  Median: " << medianDealloc << " ns" << std::endl;
        std::cout << "  Average:" << avgDealloc << " ns" << std::endl;
        std::cout << "  Total deallocations: " << deallocationLatencies.size() << std::endl;
    }
}

// Compare with standard new/delete
void compareWithStandardAllocation() {
    std::cout << "\n=== Comparison with Standard new/delete ===" << std::endl;
    
    const size_t iterations = 100000;
    
    // Test MemoryPool
    MemoryPool<PerformanceTestObject> pool(iterations);
    PerformanceTimer timer;
    
    std::cout << "Testing MemoryPool performance..." << std::endl;
    timer.start();
    for (size_t i = 0; i < iterations; ++i) {
        PerformanceTestObject* obj = pool.allocate();
        if (obj) {
            *obj = PerformanceTestObject(static_cast<int>(i), i * 3.14159);
            pool.deallocate(obj);
        }
    }
    timer.stop();
    double poolTime = timer.getMicroseconds();
    
    // Test standard new/delete
    std::cout << "Testing standard new/delete performance..." << std::endl;
    timer.start();
    for (size_t i = 0; i < iterations; ++i) {
        PerformanceTestObject* obj = new PerformanceTestObject(static_cast<int>(i), i * 3.14159);
        delete obj;
    }
    timer.stop();
    double standardTime = timer.getMicroseconds();
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Performance Comparison (" << iterations << " allocate/deallocate cycles):" << std::endl;
    std::cout << "  MemoryPool: " << poolTime << " microseconds" << std::endl;
    std::cout << "  new/delete: " << standardTime << " microseconds" << std::endl;
    std::cout << "  Speedup:    " << (standardTime / poolTime) << "x faster" << std::endl;
    std::cout << "  Time per operation:" << std::endl;
    std::cout << "    MemoryPool: " << (poolTime / iterations) << " microseconds" << std::endl;
    std::cout << "    new/delete: " << (standardTime / iterations) << " microseconds" << std::endl;
}

// Test different object sizes
void testDifferentObjectSizes() {
    std::cout << "\n=== Different Object Sizes Test ===" << std::endl;
    
    const size_t iterations = 100000;
    
    // Test with different object types
    struct SmallObject { int x; };
    struct MediumObject { int x; double y; char data[32]; };
    struct LargeObject { int x; double y; char data[256]; };
    
    std::cout << "Testing with different object sizes..." << std::endl;
    
    // Small objects
    MemoryPool<SmallObject> smallPool(iterations);
    PerformanceTimer timer;
    timer.start();
    for (size_t i = 0; i < iterations; ++i) {
        SmallObject* obj = smallPool.allocate();
        if (obj) {
            obj->x = static_cast<int>(i);
            smallPool.deallocate(obj);
        }
    }
    timer.stop();
    double smallTime = timer.getMicroseconds();
    
    // Medium objects
    MemoryPool<MediumObject> mediumPool(iterations);
    timer.start();
    for (size_t i = 0; i < iterations; ++i) {
        MediumObject* obj = mediumPool.allocate();
        if (obj) {
            obj->x = static_cast<int>(i);
            obj->y = i * 3.14159;
            mediumPool.deallocate(obj);
        }
    }
    timer.stop();
    double mediumTime = timer.getMicroseconds();
    
    // Large objects
    MemoryPool<LargeObject> largePool(iterations);
    timer.start();
    for (size_t i = 0; i < iterations; ++i) {
        LargeObject* obj = largePool.allocate();
        if (obj) {
            obj->x = static_cast<int>(i);
            obj->y = i * 3.14159;
            largePool.deallocate(obj);
        }
    }
    timer.stop();
    double largeTime = timer.getMicroseconds();
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Object Size Performance (" << iterations << " allocate/deallocate cycles):" << std::endl;
    std::cout << "  Small objects (" << sizeof(SmallObject) << " bytes): " << smallTime << " microseconds" << std::endl;
    std::cout << "  Medium objects (" << sizeof(MediumObject) << " bytes): " << mediumTime << " microseconds" << std::endl;
    std::cout << "  Large objects (" << sizeof(LargeObject) << " bytes): " << largeTime << " microseconds" << std::endl;
}

int main() {
    std::cout << "MemoryPool Performance Test Suite" << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        testAllocationLatency();
        testDeallocationLatency();
        testMixedOperationsLatency();
        compareWithStandardAllocation();
        testDifferentObjectSizes();
        
        std::cout << "\n=== Performance Test Complete ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during performance testing: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
