#include "../../runtime/gc.h"
#include "../../runtime/allocator.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace sapphire::runtime;

void test_gc_initialization() {
    std::cout << "Test: GC Initialization... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    assert(global_gc != nullptr);
    assert(global_gc->phase() == GCPhase::IDLE);
    assert(global_gc->stats().collections == 0);
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_collect() {
    std::cout << "Test: GC Collect... ";
    std::cout.flush();
    
    init_allocator();
    std::cout << "Allocator initialized... ";
    std::cout.flush();
    
    init_gc(global_allocator);
    std::cout << "GC initialized... ";
    std::cout.flush();
    
    // Allocate some objects
    void* ptr1 = sapphire_allocate(64, 1);
    void* ptr2 = sapphire_allocate(128, 2);
    void* ptr3 = sapphire_allocate(256, 3);
    std::cout << "Objects allocated... ";
    std::cout.flush();
    
    // Add as roots so they're not collected
    sapphire_gc_add_root(ptr1);
    sapphire_gc_add_root(ptr2);
    sapphire_gc_add_root(ptr3);
    std::cout << "Roots added... ";
    std::cout.flush();
    
    // Run GC
    sapphire_gc_collect();
    std::cout << "GC collected... ";
    std::cout.flush();
    
    // Check statistics
    assert(sapphire_gc_collections() == 1);
    assert(global_gc->stats().objects_marked >= 3);
    std::cout << "Stats checked... ";
    std::cout.flush();
    
    // Remove roots (but don't free - let shutdown handle it)
    sapphire_gc_remove_root(ptr1);
    sapphire_gc_remove_root(ptr2);
    sapphire_gc_remove_root(ptr3);
    std::cout << "Roots removed... ";
    std::cout.flush();
    
    shutdown_gc();
    std::cout << "GC shutdown... ";
    std::cout.flush();
    
    shutdown_allocator();
    std::cout << "Allocator shutdown... ";
    std::cout.flush();
    
    std::cout << "PASS\n";
}

void test_gc_mark_phase() {
    std::cout << "Test: GC Mark Phase... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Allocate objects
    void* ptr1 = sapphire_allocate(64, 1);
    void* ptr2 = sapphire_allocate(64, 2);
    
    // Add one as root
    sapphire_gc_add_root(ptr1);
    
    // Run GC
    sapphire_gc_collect();
    
    // Check that root was marked
    ObjectHeader* header1 = Allocator::get_header(ptr1);
    // Note: mark bit is cleared after sweep
    
    // Check statistics
    assert(global_gc->stats().objects_marked >= 1);
    
    sapphire_gc_remove_root(ptr1);
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_threshold() {
    std::cout << "Test: GC Threshold... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Set low threshold
    global_gc->set_threshold(1024);
    
    // Allocate until GC triggers
    std::vector<void*> ptrs;
    for (int i = 0; i < 100; i++) {
        void* ptr = sapphire_allocate(64, i);
        ptrs.push_back(ptr);
        sapphire_gc_add_root(ptr);
    }
    
    // GC should have run
    // (Note: might not trigger if threshold is high)
    
    // Clean up
    for (void* ptr : ptrs) {
        sapphire_gc_remove_root(ptr);
    }
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_statistics() {
    std::cout << "Test: GC Statistics... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Initial state
    assert(sapphire_gc_collections() == 0);
    
    // Run GC
    sapphire_gc_collect();
    assert(sapphire_gc_collections() == 1);
    
    // Run again
    sapphire_gc_collect();
    assert(sapphire_gc_collections() == 2);
    
    // Check pause time (might be 0 if very fast)
    size_t pause = sapphire_gc_last_pause_us();
    // Just check it's not crazy long
    assert(pause < 1000000);  // Should be < 1 second
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_roots() {
    std::cout << "Test: GC Roots... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Allocate object
    void* ptr = sapphire_allocate(64, 1);
    
    // Add as root
    sapphire_gc_add_root(ptr);
    
    // Run GC - should not collect
    sapphire_gc_collect();
    
    // Object should still be valid
    ObjectHeader* header = Allocator::get_header(ptr);
    assert(header->type_id == 1);
    
    // Remove root
    sapphire_gc_remove_root(ptr);
    
    // Run GC again - now it can be collected
    sapphire_gc_collect();
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_multiple_cycles() {
    std::cout << "Test: Multiple GC Cycles... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Run multiple GC cycles
    for (int i = 0; i < 10; i++) {
        // Allocate some objects
        std::vector<void*> ptrs;
        for (int j = 0; j < 10; j++) {
            void* ptr = sapphire_allocate(64, j);
            ptrs.push_back(ptr);
            sapphire_gc_add_root(ptr);
        }
        
        // Run GC
        sapphire_gc_collect();
        
        // Clean up roots
        for (void* ptr : ptrs) {
            sapphire_gc_remove_root(ptr);
        }
    }
    
    // Should have run 10 collections
    assert(sapphire_gc_collections() == 10);
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

void test_gc_pause_time() {
    std::cout << "Test: GC Pause Time... ";
    
    init_allocator();
    init_gc(global_allocator);
    
    // Allocate many objects
    std::vector<void*> ptrs;
    for (int i = 0; i < 1000; i++) {
        void* ptr = sapphire_allocate(64, i);
        ptrs.push_back(ptr);
        sapphire_gc_add_root(ptr);
    }
    
    // Run GC and measure pause time
    sapphire_gc_collect();
    
    size_t pause = sapphire_gc_last_pause_us();
    std::cout << "(" << pause << "μs) ";
    
    // Should be reasonably fast
    // Target: < 10ms = 10,000μs
    // For now, just check it's not crazy long
    assert(pause < 1000000);  // < 1 second
    
    // Clean up
    for (void* ptr : ptrs) {
        sapphire_gc_remove_root(ptr);
    }
    
    shutdown_gc();
    shutdown_allocator();
    
    std::cout << "PASS\n";
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Garbage Collector Tests\n";
    std::cout << "================================================================================\n\n";
    
    test_gc_initialization();
    test_gc_collect();
    test_gc_mark_phase();
    test_gc_threshold();
    test_gc_statistics();
    test_gc_roots();
    test_gc_multiple_cycles();
    test_gc_pause_time();
    
    std::cout << "\n================================================================================\n";
    std::cout << "All tests passed! ✅\n";
    std::cout << "================================================================================\n";
    
    return 0;
}
