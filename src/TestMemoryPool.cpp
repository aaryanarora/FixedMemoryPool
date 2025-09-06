#include "MemoryPool.h"
#include <iostream>
#include <vector>
#include <cassert>

// Example class to demonstrate MemoryPool usage
class TestObject {
public:
    int value;
    double data;

    TestObject(int v = 0, double d = 0.0) : value(v), data(d) {}

    void print() const {
        std::cout << "TestObject: value=" << value << ", data=" << data << std::endl;
    }
};

void test_basic_allocation() {
    std::cout << "=== Basic Allocation/Deallocation Test ===" << std::endl;
    MemoryPool<TestObject> pool(3);

    TestObject* obj1 = pool.allocate();
    assert(obj1 != nullptr);
    *obj1 = TestObject(1, 1.1);
    obj1->print();

    TestObject* obj2 = pool.allocate();
    assert(obj2 != nullptr);
    *obj2 = TestObject(2, 2.2);
    obj2->print();

    std::cout << "Available slots after 2 allocations: " << pool.getAvailableSlots() << std::endl;

    pool.deallocate(obj1);
    std::cout << "Available slots after deallocating obj1: " << pool.getAvailableSlots() << std::endl;

    pool.deallocate(obj2);
    std::cout << "Available slots after deallocating obj2: " << pool.getAvailableSlots() << std::endl;
}

void test_full_pool() {
    std::cout << "\n=== Full Pool Test ===" << std::endl;
    const size_t poolSize = 5;
    MemoryPool<TestObject> pool(poolSize);
    std::vector<TestObject*> objects;

    // Fill the pool
    for (size_t i = 0; i < poolSize; ++i) {
        TestObject* obj = pool.allocate();
        assert(obj != nullptr);
        *obj = TestObject(static_cast<int>(i), i * 1.5);
        objects.push_back(obj);
    }
    std::cout << "Available slots after filling pool: " << pool.getAvailableSlots() << std::endl;
    assert(pool.isFull());

    // Try to allocate one more (should fail)
    TestObject* extra = pool.allocate();
    if (!extra) {
        std::cout << "Allocation failed as expected when pool is full." << std::endl;
    } else {
        std::cout << "ERROR: Allocation succeeded when pool should be full!" << std::endl;
    }

    // Deallocate all
    for (auto obj : objects) {
        pool.deallocate(obj);
    }
    std::cout << "Available slots after deallocating all: " << pool.getAvailableSlots() << std::endl;
    assert(pool.isEmpty());
}

void test_empty_pool() {
    std::cout << "\n=== Empty Pool Test ===" << std::endl;
    MemoryPool<TestObject> pool(2);

    // Pool is empty at start
    assert(pool.isEmpty());
    std::cout << "Pool is empty at start: " << std::boolalpha << pool.isEmpty() << std::endl;

    // Deallocate nullptr (should not crash)
    pool.deallocate(nullptr);

    // Allocate one, deallocate, check empty again
    TestObject* obj = pool.allocate();
    assert(obj != nullptr);
    pool.deallocate(obj);
    assert(pool.isEmpty());
    std::cout << "Pool is empty after deallocation: " << std::boolalpha << pool.isEmpty() << std::endl;
}

void test_invalid_deallocation() {
    std::cout << "\n=== Invalid Deallocation Test ===" << std::endl;
    MemoryPool<TestObject> pool(2);
    TestObject stackObj;
    try {
        pool.deallocate(&stackObj); // Should throw
        std::cout << "ERROR: Invalid deallocation did not throw!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Caught expected exception: " << e.what() << std::endl;
    }
}

void test_multiple_alloc_dealloc() {
    std::cout << "\n=== Multiple Alloc/Dealloc Test ===" << std::endl;
    MemoryPool<TestObject> pool(4);
    std::vector<TestObject*> ptrs;

    // Allocate all
    for (int i = 0; i < 4; ++i) {
        TestObject* obj = pool.allocate();
        assert(obj != nullptr);
        *obj = TestObject(i * 10, i * 2.5);
        ptrs.push_back(obj);
    }
    assert(pool.isFull());

    // Deallocate in reverse order
    for (int i = 3; i >= 0; --i) {
        pool.deallocate(ptrs[i]);
    }
    assert(pool.isEmpty());
    std::cout << "Multiple alloc/dealloc test passed." << std::endl;
}

int main()
{
    test_basic_allocation();
    test_full_pool();
    test_empty_pool();
    test_invalid_deallocation();
    test_multiple_alloc_dealloc();

    std::cout << "\nAll tests completed." << std::endl;
    return 0;
}