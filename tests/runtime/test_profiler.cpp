#include "runtime/profiler.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>

using namespace sapphire::runtime;

// Test counters
int tests_passed = 0;
int tests_failed = 0;

// Test object
struct TestObject {
    char data[64];
};

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " #name "..." << std::endl; \
        MemoryProfiler::reset(); \
        try { \
            test_##name(); \
            tests_passed++; \
            std::cout << "  ✓ PASSED" << std::endl; \
        } catch (const std::exception& e) { \
            tests_failed++; \
            std::cout << "  ✗ FAILED: " << e.what() << std::endl; \
        } \
    } \
    void test_##name()

//=============================================================================
// Test: Enable/Disable Profiler
//=============================================================================

TEST(enable_disable) {
    std::cout << "  Testing enable/disable..." << std::endl;
    
    assert(!MemoryProfiler::is_enabled());
    
    MemoryProfiler::enable();
    assert(MemoryProfiler::is_enabled());
    
    MemoryProfiler::disable();
    assert(!MemoryProfiler::is_enabled());
    
    std::cout << "  Enable/disable working correctly" << std::endl;
}

//=============================================================================
// Test: Track Allocation
//=============================================================================

TEST(track_allocation) {
    std::cout << "  Testing allocation tracking..." << std::endl;
    
    MemoryProfiler::enable();
    
    TestObject* obj = new TestObject();
    AllocationTracker::track_allocation(obj, sizeof(TestObject), 1);
    
    assert(AllocationTracker::is_tracked(obj));
    assert(AllocationTracker::total_allocated() == sizeof(TestObject));
    assert(AllocationTracker::current_usage() == sizeof(TestObject));
    assert(AllocationTracker::allocation_count() == 1);
    
    delete obj;
    
    std::cout << "  Allocation tracking working correctly" << std::endl;
}

//=============================================================================
// Test: Track Free
//=============================================================================

TEST(track_free) {
    std::cout << "  Testing free tracking..." << std::endl;
    
    MemoryProfiler::enable();
    
    TestObject* obj = new TestObject();
    AllocationTracker::track_allocation(obj, sizeof(TestObject), 1);
    AllocationTracker::track_free(obj);
    
    assert(AllocationTracker::total_freed() == sizeof(TestObject));
    assert(AllocationTracker::current_usage() == 0);
    assert(AllocationTracker::free_count() == 1);
    
    delete obj;
    
    std::cout << "  Free tracking working correctly" << std::endl;
}

//=============================================================================
// Test: Memory Statistics
//=============================================================================

TEST(memory_statistics) {
    std::cout << "  Testing memory statistics..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate some objects
    TestObject* obj1 = new TestObject();
    TestObject* obj2 = new TestObject();
    TestObject* obj3 = new TestObject();
    
    AllocationTracker::track_allocation(obj1, sizeof(TestObject), 1);
    AllocationTracker::track_allocation(obj2, sizeof(TestObject), 2);
    AllocationTracker::track_allocation(obj3, sizeof(TestObject), 1);
    
    MemoryStats stats = MemoryProfiler::get_stats();
    
    assert(stats.total_allocated == 3 * sizeof(TestObject));
    assert(stats.allocation_count == 3);
    assert(stats.current_usage == 3 * sizeof(TestObject));
    assert(stats.peak_usage == 3 * sizeof(TestObject));
    
    // Free one
    AllocationTracker::track_free(obj2);
    delete obj2;
    
    stats = MemoryProfiler::get_stats();
    assert(stats.total_freed == sizeof(TestObject));
    assert(stats.current_usage == 2 * sizeof(TestObject));
    assert(stats.free_count == 1);
    
    delete obj1;
    delete obj3;
    
    std::cout << "  Memory statistics working correctly" << std::endl;
}

//=============================================================================
// Test: Leak Detection
//=============================================================================

TEST(leak_detection) {
    std::cout << "  Testing leak detection..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate without freeing (leak)
    TestObject* obj1 = new TestObject();
    AllocationTracker::track_allocation(obj1, sizeof(TestObject), 1);
    
    // Allocate and free (no leak)
    TestObject* obj2 = new TestObject();
    AllocationTracker::track_allocation(obj2, sizeof(TestObject), 2);
    AllocationTracker::track_free(obj2);
    delete obj2;
    
    // Check leaks
    assert(AllocationTracker::leak_count() == 1);
    assert(AllocationTracker::is_leaked(obj1));
    assert(!AllocationTracker::is_leaked(obj2));
    
    auto leaks = LeakDetector::find_leaks();
    assert(leaks.size() == 1);
    assert(leaks[0] == obj1);
    
    delete obj1;
    
    std::cout << "  Leak detection working correctly" << std::endl;
}

//=============================================================================
// Test: Peak Usage
//=============================================================================

TEST(peak_usage) {
    std::cout << "  Testing peak usage tracking..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate 3 objects
    TestObject* obj1 = new TestObject();
    TestObject* obj2 = new TestObject();
    TestObject* obj3 = new TestObject();
    
    AllocationTracker::track_allocation(obj1, sizeof(TestObject), 1);
    AllocationTracker::track_allocation(obj2, sizeof(TestObject), 1);
    AllocationTracker::track_allocation(obj3, sizeof(TestObject), 1);
    
    size_t peak = AllocationTracker::peak_usage();
    assert(peak == 3 * sizeof(TestObject));
    
    // Free 2 objects
    AllocationTracker::track_free(obj1);
    AllocationTracker::track_free(obj2);
    delete obj1;
    delete obj2;
    
    // Peak should still be 3 objects
    assert(AllocationTracker::peak_usage() == peak);
    assert(AllocationTracker::current_usage() == sizeof(TestObject));
    
    delete obj3;
    
    std::cout << "  Peak usage tracking working correctly" << std::endl;
}

//=============================================================================
// Test: Allocations by Size
//=============================================================================

TEST(allocations_by_size) {
    std::cout << "  Testing allocations by size..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate different sizes
    char* small = new char[16];
    char* medium = new char[64];
    char* large = new char[256];
    
    AllocationTracker::track_allocation(small, 16, 1);
    AllocationTracker::track_allocation(medium, 64, 1);
    AllocationTracker::track_allocation(large, 256, 1);
    
    MemoryStats stats = MemoryProfiler::get_stats();
    
    assert(stats.allocations_by_size[16] == 1);
    assert(stats.allocations_by_size[64] == 1);
    assert(stats.allocations_by_size[256] == 1);
    
    delete[] small;
    delete[] medium;
    delete[] large;
    
    std::cout << "  Allocations by size working correctly" << std::endl;
}

//=============================================================================
// Test: Allocations by Type
//=============================================================================

TEST(allocations_by_type) {
    std::cout << "  Testing allocations by type..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate different types
    TestObject* obj1 = new TestObject();
    TestObject* obj2 = new TestObject();
    TestObject* obj3 = new TestObject();
    
    AllocationTracker::track_allocation(obj1, sizeof(TestObject), 1);
    AllocationTracker::track_allocation(obj2, sizeof(TestObject), 2);
    AllocationTracker::track_allocation(obj3, sizeof(TestObject), 1);
    
    MemoryStats stats = MemoryProfiler::get_stats();
    
    assert(stats.allocations_by_type[1] == 2);
    assert(stats.allocations_by_type[2] == 1);
    
    delete obj1;
    delete obj2;
    delete obj3;
    
    std::cout << "  Allocations by type working correctly" << std::endl;
}

//=============================================================================
// Test: Reset Profiler
//=============================================================================

TEST(reset_profiler) {
    std::cout << "  Testing profiler reset..." << std::endl;
    
    MemoryProfiler::enable();
    
    // Allocate some objects
    TestObject* obj = new TestObject();
    AllocationTracker::track_allocation(obj, sizeof(TestObject), 1);
    
    assert(AllocationTracker::allocation_count() > 0);
    
    // Reset
    MemoryProfiler::reset();
    
    assert(AllocationTracker::allocation_count() == 0);
    assert(AllocationTracker::total_allocated() == 0);
    assert(AllocationTracker::current_usage() == 0);
    
    delete obj;
    
    std::cout << "  Profiler reset working correctly" << std::endl;
}

//=============================================================================
// Test: Thread Safety
//=============================================================================

TEST(thread_safety) {
    std::cout << "  Testing thread safety..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    const int num_threads = 10;
    const int allocs_per_thread = 100;
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([allocs_per_thread]() {
            for (int j = 0; j < allocs_per_thread; j++) {
                TestObject* obj = new TestObject();
                AllocationTracker::track_allocation(obj, sizeof(TestObject), 1);
                AllocationTracker::track_free(obj);
                delete obj;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Check that all allocations were tracked
    size_t total_allocated = AllocationTracker::total_allocated();
    size_t total_freed = AllocationTracker::total_freed();
    size_t expected = num_threads * allocs_per_thread * sizeof(TestObject);
    
    assert(total_allocated == expected);
    assert(total_freed == expected);
    assert(AllocationTracker::current_usage() == 0);
    
    std::cout << "  Thread safety working correctly" << std::endl;
}

//=============================================================================
// Test: C API
//=============================================================================

TEST(c_api) {
    std::cout << "  Testing C API..." << std::endl;
    
    sapphire_profiler_enable();
    assert(sapphire_profiler_is_enabled());
    
    sapphire_profiler_reset();
    
    TestObject* obj = new TestObject();
    sapphire_profiler_track_allocation(obj, sizeof(TestObject), 1);
    
    assert(sapphire_profiler_total_allocated() == sizeof(TestObject));
    assert(sapphire_profiler_current_usage() == sizeof(TestObject));
    assert(sapphire_profiler_leak_count() == 1);
    
    sapphire_profiler_track_free(obj);
    assert(sapphire_profiler_leak_count() == 0);
    
    delete obj;
    
    sapphire_profiler_disable();
    assert(!sapphire_profiler_is_enabled());
    
    std::cout << "  C API working correctly" << std::endl;
}

//=============================================================================
// Test: Print Reports
//=============================================================================

TEST(print_reports) {
    std::cout << "  Testing print reports..." << std::endl;
    
    MemoryProfiler::enable();
    MemoryProfiler::reset();
    
    // Allocate some objects
    TestObject* obj1 = new TestObject();
    TestObject* obj2 = new TestObject();
    
    AllocationTracker::track_allocation(obj1, sizeof(TestObject), 1);
    AllocationTracker::track_allocation(obj2, sizeof(TestObject), 2);
    
    // Print stats (should not crash)
    std::cout << "\n";
    MemoryProfiler::print_stats();
    
    // Print leaks (should show 2 leaks)
    std::cout << "\n";
    MemoryProfiler::print_leaks();
    
    // Print summary
    std::cout << "\n";
    MemoryProfiler::print_summary();
    std::cout << "\n";
    
    delete obj1;
    delete obj2;
    
    std::cout << "  Print reports working correctly" << std::endl;
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Memory Profiler Tests\n";
    std::cout << "================================================================================\n\n";
    
    // Run all tests
    run_test_enable_disable();
    run_test_track_allocation();
    run_test_track_free();
    run_test_memory_statistics();
    run_test_leak_detection();
    run_test_peak_usage();
    run_test_allocations_by_size();
    run_test_allocations_by_type();
    run_test_reset_profiler();
    run_test_thread_safety();
    run_test_c_api();
    run_test_print_reports();
    
    // Print summary
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Memory profiler is working correctly! 📊\n";
    } else {
        std::cout << "\n✗ Some tests failed. Please review the output above.\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
