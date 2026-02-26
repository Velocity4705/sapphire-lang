#include "runtime/ownership.h"
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
    int value;
    TestObject(int v = 0) : value(v) {}
};

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " #name "..." << std::endl; \
        OwnershipTracker::clear(); \
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

#define EXPECT_THROW(expr, exception_type) \
    do { \
        bool caught = false; \
        try { \
            expr; \
        } catch (const exception_type& e) { \
            caught = true; \
            std::cout << "    Expected exception caught: " << e.what() << std::endl; \
        } catch (const std::exception& e) { \
            std::cout << "    Wrong exception type: " << e.what() << std::endl; \
            throw; \
        } \
        assert(caught && "Expected exception was not thrown"); \
    } while(0)

#define EXPECT_NO_THROW(expr) \
    do { \
        try { \
            expr; \
        } catch (const std::exception& e) { \
            std::cout << "    Unexpected exception: " << e.what() << std::endl; \
            throw; \
        } \
    } while(0)

//=============================================================================
// Test: Basic Ownership Tracking
//=============================================================================

TEST(basic_ownership) {
    std::cout << "  Testing basic ownership tracking..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner = OwnershipTracker::current_thread_id();
    
    // Set owner
    OwnershipTracker::set_owner(&obj, owner);
    
    // Check ownership
    assert(OwnershipTracker::has_owner(&obj));
    assert(OwnershipTracker::get_owner(&obj) == owner);
    assert(OwnershipTracker::is_owner(&obj, owner));
    assert(OwnershipTracker::tracked_count() == 1);
    
    // Untrack
    OwnershipTracker::untrack(&obj);
    assert(!OwnershipTracker::has_owner(&obj));
    assert(OwnershipTracker::tracked_count() == 0);
    
    std::cout << "  Basic ownership tracking working correctly" << std::endl;
}

//=============================================================================
// Test: Move Semantics
//=============================================================================

TEST(move_semantics) {
    std::cout << "  Testing move semantics..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner = OwnershipTracker::current_thread_id();
    
    OwnershipTracker::set_owner(&obj, owner);
    assert(!OwnershipTracker::is_moved(&obj));
    assert(OwnershipTracker::get_move_count(&obj) == 0);
    
    // Mark as moved
    OwnershipTracker::mark_moved(&obj);
    assert(OwnershipTracker::is_moved(&obj));
    assert(OwnershipTracker::get_move_count(&obj) == 1);
    
    std::cout << "  Move semantics working correctly" << std::endl;
}

//=============================================================================
// Test: Use-After-Move Detection
//=============================================================================

TEST(use_after_move) {
    std::cout << "  Testing use-after-move detection..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner = OwnershipTracker::current_thread_id();
    
    OwnershipTracker::set_owner(&obj, owner);
    
    // Should not throw before move
    EXPECT_NO_THROW(OwnershipTracker::check_not_moved(&obj));
    
    // Mark as moved
    OwnershipTracker::mark_moved(&obj);
    
    // Should throw after move
    EXPECT_THROW(OwnershipTracker::check_not_moved(&obj), UseAfterMoveError);
    
    std::cout << "  Use-after-move detection working correctly" << std::endl;
}

//=============================================================================
// Test: Ownership Transfer
//=============================================================================

TEST(ownership_transfer) {
    std::cout << "  Testing ownership transfer..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner1 = 100;
    uint64_t owner2 = 200;
    
    // Set initial owner
    OwnershipTracker::set_owner(&obj, owner1);
    assert(OwnershipTracker::get_owner(&obj) == owner1);
    assert(OwnershipTracker::get_move_count(&obj) == 0);
    
    // Transfer ownership
    OwnershipTracker::transfer_ownership(&obj, owner2);
    assert(OwnershipTracker::get_owner(&obj) == owner2);
    assert(OwnershipTracker::get_move_count(&obj) == 1);
    assert(!OwnershipTracker::is_moved(&obj));  // Reset after transfer
    
    std::cout << "  Ownership transfer working correctly" << std::endl;
}

//=============================================================================
// Test: Ownership Violation
//=============================================================================

TEST(ownership_violation) {
    std::cout << "  Testing ownership violation detection..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner1 = 100;
    uint64_t owner2 = 200;
    
    OwnershipTracker::set_owner(&obj, owner1);
    
    // Owner can access
    EXPECT_NO_THROW(OwnershipTracker::check_ownership(&obj, owner1));
    
    // Non-owner cannot access
    EXPECT_THROW(OwnershipTracker::check_ownership(&obj, owner2), OwnershipViolationError);
    
    std::cout << "  Ownership violation detection working correctly" << std::endl;
}

//=============================================================================
// Test: Shared Ownership
//=============================================================================

TEST(shared_ownership) {
    std::cout << "  Testing shared ownership..." << std::endl;
    
    TestObject obj(42);
    
    // Mark as shared
    OwnershipTracker::mark_shared(&obj);
    assert(OwnershipTracker::is_shared(&obj));
    assert(OwnershipTracker::get_owner(&obj) == 0);  // No single owner
    assert(OwnershipTracker::shared_count() == 1);
    
    // Shared objects can be accessed by anyone
    EXPECT_NO_THROW(OwnershipTracker::check_ownership(&obj, 100));
    EXPECT_NO_THROW(OwnershipTracker::check_ownership(&obj, 200));
    
    std::cout << "  Shared ownership working correctly" << std::endl;
}

//=============================================================================
// Test: Owned<T> Wrapper
//=============================================================================

TEST(owned_wrapper) {
    std::cout << "  Testing Owned<T> wrapper..." << std::endl;
    
    {
        Owned<TestObject> owned(new TestObject(42));
        assert(owned);
        assert(owned->value == 42);
        assert(OwnershipTracker::tracked_count() == 1);
    }
    
    // Object should be deleted and untracked
    assert(OwnershipTracker::tracked_count() == 0);
    
    std::cout << "  Owned<T> wrapper working correctly" << std::endl;
}

//=============================================================================
// Test: Owned<T> Move
//=============================================================================

TEST(owned_move) {
    std::cout << "  Testing Owned<T> move..." << std::endl;
    
    Owned<TestObject> owned1(new TestObject(42));
    TestObject* ptr = owned1.get();
    
    uint32_t move_count_before = OwnershipTracker::get_move_count(ptr);
    
    // Move to owned2
    Owned<TestObject> owned2 = std::move(owned1);
    
    assert(!owned1);  // owned1 is now null
    assert(owned2);
    assert(owned2->value == 42);
    
    uint32_t move_count_after = OwnershipTracker::get_move_count(ptr);
    assert(move_count_after == move_count_before + 1);  // Move count increased
    
    std::cout << "  Owned<T> move working correctly" << std::endl;
}

//=============================================================================
// Test: Owned<T> Release
//=============================================================================

TEST(owned_release) {
    std::cout << "  Testing Owned<T> release..." << std::endl;
    
    Owned<TestObject> owned(new TestObject(42));
    TestObject* ptr = owned.release();
    
    assert(!owned);  // owned is now null
    assert(ptr != nullptr);
    assert(ptr->value == 42);
    assert(!OwnershipTracker::has_owner(ptr));  // Untracked after release
    
    delete ptr;  // Manual cleanup
    
    std::cout << "  Owned<T> release working correctly" << std::endl;
}

//=============================================================================
// Test: Owned<T> Reset
//=============================================================================

TEST(owned_reset) {
    std::cout << "  Testing Owned<T> reset..." << std::endl;
    
    Owned<TestObject> owned(new TestObject(42));
    assert(owned);
    assert(OwnershipTracker::tracked_count() == 1);
    
    owned.reset(new TestObject(99));
    assert(owned);
    assert(owned->value == 99);
    assert(OwnershipTracker::tracked_count() == 1);
    
    owned.reset();
    assert(!owned);
    assert(OwnershipTracker::tracked_count() == 0);
    
    std::cout << "  Owned<T> reset working correctly" << std::endl;
}

//=============================================================================
// Test: Multiple Objects
//=============================================================================

TEST(multiple_objects) {
    std::cout << "  Testing multiple objects..." << std::endl;
    
    TestObject obj1(1);
    TestObject obj2(2);
    TestObject obj3(3);
    
    uint64_t owner = OwnershipTracker::current_thread_id();
    
    OwnershipTracker::set_owner(&obj1, owner);
    OwnershipTracker::set_owner(&obj2, owner);
    OwnershipTracker::set_owner(&obj3, owner);
    
    assert(OwnershipTracker::tracked_count() == 3);
    
    OwnershipTracker::mark_moved(&obj2);
    assert(OwnershipTracker::moved_count() == 1);
    
    OwnershipTracker::mark_shared(&obj3);
    assert(OwnershipTracker::shared_count() == 1);
    
    std::cout << "  Multiple objects working correctly" << std::endl;
}

//=============================================================================
// Test: Thread Safety
//=============================================================================

TEST(thread_safety) {
    std::cout << "  Testing thread safety..." << std::endl;
    
    const int num_threads = 10;
    const int objects_per_thread = 100;
    
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([objects_per_thread]() {
            uint64_t owner = OwnershipTracker::current_thread_id();
            
            for (int j = 0; j < objects_per_thread; j++) {
                TestObject* obj = new TestObject(j);
                OwnershipTracker::set_owner(obj, owner);
                assert(OwnershipTracker::is_owner(obj, owner));
                OwnershipTracker::untrack(obj);
                delete obj;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(OwnershipTracker::tracked_count() == 0);
    
    std::cout << "  Thread safety working correctly" << std::endl;
}

//=============================================================================
// Test: C API
//=============================================================================

TEST(c_api) {
    std::cout << "  Testing C API..." << std::endl;
    
    TestObject obj(42);
    uint64_t owner = sapphire_current_thread_id();
    
    // Set owner
    sapphire_set_owner(&obj, owner);
    assert(sapphire_get_owner(&obj) == owner);
    assert(sapphire_is_owner(&obj, owner));
    
    // Move
    assert(!sapphire_is_moved(&obj));
    sapphire_mark_moved(&obj);
    assert(sapphire_is_moved(&obj));
    
    // Transfer
    uint64_t new_owner = owner + 1;
    sapphire_transfer_ownership(&obj, new_owner);
    assert(sapphire_get_owner(&obj) == new_owner);
    
    // Shared
    sapphire_mark_shared(&obj);
    assert(sapphire_is_shared(&obj));
    
    std::cout << "  C API working correctly" << std::endl;
}

//=============================================================================
// Test: Statistics
//=============================================================================

TEST(statistics) {
    std::cout << "  Testing statistics..." << std::endl;
    
    TestObject obj1(1), obj2(2), obj3(3), obj4(4);
    uint64_t owner = OwnershipTracker::current_thread_id();
    
    OwnershipTracker::set_owner(&obj1, owner);
    OwnershipTracker::set_owner(&obj2, owner);
    OwnershipTracker::set_owner(&obj3, owner);
    OwnershipTracker::set_owner(&obj4, owner);
    
    assert(OwnershipTracker::tracked_count() == 4);
    assert(OwnershipTracker::moved_count() == 0);
    assert(OwnershipTracker::shared_count() == 0);
    
    OwnershipTracker::mark_moved(&obj1);
    OwnershipTracker::mark_moved(&obj2);
    assert(OwnershipTracker::moved_count() == 2);
    
    OwnershipTracker::mark_shared(&obj3);
    assert(OwnershipTracker::shared_count() == 1);
    
    OwnershipTracker::untrack(&obj4);
    assert(OwnershipTracker::tracked_count() == 3);
    
    std::cout << "  Statistics working correctly" << std::endl;
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Ownership System Tests\n";
    std::cout << "================================================================================\n\n";
    
    // Run all tests
    run_test_basic_ownership();
    run_test_move_semantics();
    run_test_use_after_move();
    run_test_ownership_transfer();
    run_test_ownership_violation();
    run_test_shared_ownership();
    run_test_owned_wrapper();
    run_test_owned_move();
    run_test_owned_release();
    run_test_owned_reset();
    run_test_multiple_objects();
    run_test_thread_safety();
    run_test_c_api();
    run_test_statistics();
    
    // Print summary
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Ownership system is working correctly! 🔒\n";
    } else {
        std::cout << "\n✗ Some tests failed. Please review the output above.\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
