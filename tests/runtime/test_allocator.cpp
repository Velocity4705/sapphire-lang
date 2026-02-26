#include "../../runtime/allocator.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <thread>

using namespace sapphire::runtime;

void test_basic_allocation() {
    std::cout << "Test: Basic Allocation... ";
    
    init_allocator();
    
    // Allocate small object
    void* ptr1 = sapphire_allocate(64, 1);
    assert(ptr1 != nullptr);
    
    // Check header
    ObjectHeader* header1 = Allocator::get_header(ptr1);
    assert(header1->type_id == 1);
    assert(header1->size >= 64 + sizeof(ObjectHeader));
    assert(!header1->is_large());
    
    // Allocate large object
    void* ptr2 = sapphire_allocate(1024, 2);
    assert(ptr2 != nullptr);
    
    // Check header
    ObjectHeader* header2 = Allocator::get_header(ptr2);
    assert(header2->type_id == 2);
    assert(header2->size >= 1024 + sizeof(ObjectHeader));
    assert(header2->is_large());
    
    // Free objects
    sapphire_free(ptr1);
    sapphire_free(ptr2);
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_multiple_allocations() {
    std::cout << "Test: Multiple Allocations... ";
    
    init_allocator();
    
    std::vector<void*> ptrs;
    
    // Allocate many objects
    for (int i = 0; i < 1000; i++) {
        void* ptr = sapphire_allocate(32, i);
        assert(ptr != nullptr);
        ptrs.push_back(ptr);
    }
    
    // Verify all allocations
    for (size_t i = 0; i < ptrs.size(); i++) {
        ObjectHeader* header = Allocator::get_header(ptrs[i]);
        assert(header->type_id == i);
    }
    
    // Free all
    for (void* ptr : ptrs) {
        sapphire_free(ptr);
    }
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_size_classes() {
    std::cout << "Test: Size Classes... ";
    
    init_allocator();
    
    // Test each size class
    size_t sizes[] = {8, 16, 32, 64, 128, 256};
    for (size_t size : sizes) {
        void* ptr = sapphire_allocate(size - sizeof(ObjectHeader), 0);
        assert(ptr != nullptr);
        
        ObjectHeader* header = Allocator::get_header(ptr);
        assert(!header->is_large());
        
        sapphire_free(ptr);
    }
    
    // Test large object
    void* large_ptr = sapphire_allocate(512, 0);
    assert(large_ptr != nullptr);
    
    ObjectHeader* large_header = Allocator::get_header(large_ptr);
    assert(large_header->is_large());
    
    sapphire_free(large_ptr);
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_flags() {
    std::cout << "Test: GC Flags... ";
    
    init_allocator();
    
    void* ptr = sapphire_allocate(64, 1);
    ObjectHeader* header = Allocator::get_header(ptr);
    
    // Test mark bit
    assert(!header->is_marked());
    header->set_marked(true);
    assert(header->is_marked());
    header->set_marked(false);
    assert(!header->is_marked());
    
    // Test pinned bit
    assert(!header->is_pinned());
    header->set_pinned(true);
    assert(header->is_pinned());
    header->set_pinned(false);
    assert(!header->is_pinned());
    
    sapphire_free(ptr);
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_statistics() {
    std::cout << "Test: Statistics... ";
    
    init_allocator();
    
    size_t initial_usage = sapphire_memory_usage();
    
    // Allocate some objects
    void* ptr1 = sapphire_allocate(64, 1);
    void* ptr2 = sapphire_allocate(128, 2);
    
    size_t after_alloc = sapphire_memory_usage();
    assert(after_alloc > initial_usage);
    
    // Free one object
    sapphire_free(ptr1);
    
    size_t after_free = sapphire_memory_usage();
    assert(after_free < after_alloc);
    
    // Free second object
    sapphire_free(ptr2);
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void allocate_worker(int id, int count) {
    for (int i = 0; i < count; i++) {
        void* ptr = sapphire_allocate(64, id);
        assert(ptr != nullptr);
        
        // Do some work
        ObjectHeader* header = Allocator::get_header(ptr);
        assert(header->type_id == static_cast<uint64_t>(id));
        
        sapphire_free(ptr);
    }
}

void test_concurrent_allocation() {
    std::cout << "Test: Concurrent Allocation... ";
    
    init_allocator();
    
    const int NUM_THREADS = 4;
    const int ALLOCS_PER_THREAD = 1000;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(allocate_worker, i, ALLOCS_PER_THREAD);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_reuse() {
    std::cout << "Test: Memory Reuse... ";
    
    init_allocator();
    
    // Allocate and free many times
    for (int i = 0; i < 100; i++) {
        void* ptr = sapphire_allocate(64, 1);
        assert(ptr != nullptr);
        sapphire_free(ptr);
    }
    
    // Memory should be reused, so usage shouldn't grow much
    size_t usage = sapphire_memory_usage();
    assert(usage < 10000);  // Should be much less than 100 * 64
    
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Allocator Tests\n";
    std::cout << "================================================================================\n\n";
    
    test_basic_allocation();
    test_multiple_allocations();
    test_size_classes();
    test_gc_flags();
    test_statistics();
    test_concurrent_allocation();
    test_reuse();
    
    std::cout << "\n================================================================================\n";
    std::cout << "All tests passed! ✅\n";
    std::cout << "================================================================================\n";
    
    return 0;
}
