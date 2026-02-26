#include "runtime/refcount.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>

using namespace sapphire::runtime;

// Test counters
int tests_passed = 0;
int tests_failed = 0;

// Track object lifecycle
std::atomic<int> object_count{0};

struct TestObject {
    int value;
    
    TestObject(int v = 0) : value(v) {
        object_count.fetch_add(1);
    }
    
    ~TestObject() {
        object_count.fetch_sub(1);
    }
};

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " #name "..." << std::endl; \
        object_count.store(0); \
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
// Test: Basic Rc Creation
//=============================================================================

TEST(rc_basic_creation) {
    std::cout << "  Testing basic Rc creation..." << std::endl;
    
    {
        Rc<TestObject> rc(new TestObject(42));
        assert(rc);
        assert(rc->value == 42);
        assert(rc.use_count() == 1);
        assert(object_count.load() == 1);
    }
    
    // Object should be deleted
    assert(object_count.load() == 0);
    
    std::cout << "  Basic Rc creation working correctly" << std::endl;
}

//=============================================================================
// Test: Rc Copy
//=============================================================================

TEST(rc_copy) {
    std::cout << "  Testing Rc copy..." << std::endl;
    
    {
        Rc<TestObject> rc1(new TestObject(42));
        assert(rc1.use_count() == 1);
        
        {
            Rc<TestObject> rc2 = rc1;
            assert(rc1.use_count() == 2);
            assert(rc2.use_count() == 2);
            assert(rc1->value == 42);
            assert(rc2->value == 42);
            assert(object_count.load() == 1);
        }
        
        // rc2 destroyed, count should be 1
        assert(rc1.use_count() == 1);
        assert(object_count.load() == 1);
    }
    
    // All destroyed
    assert(object_count.load() == 0);
    
    std::cout << "  Rc copy working correctly" << std::endl;
}

//=============================================================================
// Test: Rc Move
//=============================================================================

TEST(rc_move) {
    std::cout << "  Testing Rc move..." << std::endl;
    
    {
        Rc<TestObject> rc1(new TestObject(42));
        assert(rc1.use_count() == 1);
        
        Rc<TestObject> rc2 = std::move(rc1);
        assert(!rc1);  // rc1 is now null
        assert(rc2);
        assert(rc2.use_count() == 1);
        assert(rc2->value == 42);
        assert(object_count.load() == 1);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Rc move working correctly" << std::endl;
}

//=============================================================================
// Test: Rc Clone
//=============================================================================

TEST(rc_clone) {
    std::cout << "  Testing Rc clone..." << std::endl;
    
    {
        Rc<TestObject> rc1(new TestObject(42));
        Rc<TestObject> rc2 = rc1.clone();
        
        assert(rc1.use_count() == 2);
        assert(rc2.use_count() == 2);
        assert(rc1->value == 42);
        assert(rc2->value == 42);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Rc clone working correctly" << std::endl;
}

//=============================================================================
// Test: Rc Reset
//=============================================================================

TEST(rc_reset) {
    std::cout << "  Testing Rc reset..." << std::endl;
    
    Rc<TestObject> rc(new TestObject(42));
    assert(rc);
    assert(object_count.load() == 1);
    
    rc.reset();
    assert(!rc);
    assert(object_count.load() == 0);
    
    std::cout << "  Rc reset working correctly" << std::endl;
}

//=============================================================================
// Test: Rc Make Factory
//=============================================================================

TEST(rc_make_factory) {
    std::cout << "  Testing Rc::make factory..." << std::endl;
    
    {
        auto rc = Rc<TestObject>::make(42);
        assert(rc);
        assert(rc->value == 42);
        assert(rc.use_count() == 1);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Rc::make factory working correctly" << std::endl;
}

//=============================================================================
// Test: Arc Basic
//=============================================================================

TEST(arc_basic) {
    std::cout << "  Testing basic Arc..." << std::endl;
    
    {
        Arc<TestObject> arc(new TestObject(42));
        assert(arc);
        assert(arc->value == 42);
        assert(arc.use_count() == 1);
        assert(object_count.load() == 1);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Basic Arc working correctly" << std::endl;
}

//=============================================================================
// Test: Arc Thread Safety
//=============================================================================

TEST(arc_thread_safety) {
    std::cout << "  Testing Arc thread safety..." << std::endl;
    
    Arc<TestObject> arc(new TestObject(42));
    
    const int num_threads = 10;
    const int iterations = 1000;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&arc, iterations]() {
            for (int j = 0; j < iterations; j++) {
                Arc<TestObject> local = arc;
                assert(local->value == 42);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(arc.use_count() == 1);
    assert(object_count.load() == 1);
    
    arc.reset();
    assert(object_count.load() == 0);
    
    std::cout << "  Arc thread safety working correctly" << std::endl;
}

//=============================================================================
// Test: Weak Reference
//=============================================================================

TEST(weak_reference) {
    std::cout << "  Testing weak reference..." << std::endl;
    
    Weak<TestObject>* weak_ptr = nullptr;
    
    {
        Rc<TestObject> rc(new TestObject(42));
        weak_ptr = new Weak<TestObject>(rc);
        
        assert(!weak_ptr->expired());
        assert(weak_ptr->use_count() == 1);
        assert(weak_ptr->weak_count() == 1);
        
        // Upgrade to strong reference
        Rc<TestObject> rc2 = weak_ptr->upgrade_rc();
        assert(rc2);
        assert(rc2->value == 42);
        assert(rc2.use_count() == 2);
    }
    
    // Strong references dropped, weak should be expired
    assert(weak_ptr->expired());
    assert(weak_ptr->use_count() == 0);
    
    // Try to upgrade expired weak reference
    Rc<TestObject> rc3 = weak_ptr->upgrade_rc();
    assert(!rc3);
    
    delete weak_ptr;
    assert(object_count.load() == 0);
    
    std::cout << "  Weak reference working correctly" << std::endl;
}

//=============================================================================
// Test: Weak with Arc
//=============================================================================

TEST(weak_with_arc) {
    std::cout << "  Testing weak with Arc..." << std::endl;
    
    Weak<TestObject>* weak_ptr = nullptr;
    
    {
        Arc<TestObject> arc(new TestObject(42));
        weak_ptr = new Weak<TestObject>(arc);
        
        assert(!weak_ptr->expired());
        
        // Upgrade to Arc
        Arc<TestObject> arc2 = weak_ptr->upgrade_arc();
        assert(arc2);
        assert(arc2->value == 42);
    }
    
    assert(weak_ptr->expired());
    delete weak_ptr;
    assert(object_count.load() == 0);
    
    std::cout << "  Weak with Arc working correctly" << std::endl;
}

//=============================================================================
// Test: Cycle Breaking with Weak
//=============================================================================

struct Node {
    int value;
    Rc<Node> next;  // Will be default-initialized to null
    Weak<Node>* prev;  // Weak to break cycle
    
    Node(int v) : value(v), next(), prev(nullptr) {
        object_count.fetch_add(1);
    }
    
    ~Node() {
        object_count.fetch_sub(1);
        if (prev) delete prev;
    }
};

TEST(cycle_breaking) {
    std::cout << "  Testing cycle breaking with weak references..." << std::endl;
    
    {
        // Create a cycle: A -> B -> A
        Rc<Node> a = Rc<Node>::make(1);
        Rc<Node> b = Rc<Node>::make(2);
        
        a->next = b;
        b->prev = new Weak<Node>(a);  // Weak reference breaks cycle
        
        assert(a.use_count() == 1);  // Only a owns it
        assert(b.use_count() == 2);  // a->next and b own it
        assert(object_count.load() == 2);
    }
    
    // Both should be deleted (no leak)
    assert(object_count.load() == 0);
    
    std::cout << "  Cycle breaking working correctly" << std::endl;
}

//=============================================================================
// Test: Multiple References
//=============================================================================

TEST(multiple_references) {
    std::cout << "  Testing multiple references..." << std::endl;
    
    {
        Rc<TestObject> rc1(new TestObject(42));
        Rc<TestObject> rc2 = rc1;
        Rc<TestObject> rc3 = rc1;
        Rc<TestObject> rc4 = rc2;
        Rc<TestObject> rc5 = rc3;
        
        assert(rc1.use_count() == 5);
        assert(rc2.use_count() == 5);
        assert(rc3.use_count() == 5);
        assert(rc4.use_count() == 5);
        assert(rc5.use_count() == 5);
        assert(object_count.load() == 1);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Multiple references working correctly" << std::endl;
}

//=============================================================================
// Test: Assignment
//=============================================================================

TEST(assignment) {
    std::cout << "  Testing assignment..." << std::endl;
    
    {
        Rc<TestObject> rc1(new TestObject(42));
        Rc<TestObject> rc2(new TestObject(99));
        
        assert(object_count.load() == 2);
        assert(rc1->value == 42);
        assert(rc2->value == 99);
        
        rc2 = rc1;  // rc2's object should be deleted
        
        assert(object_count.load() == 1);
        assert(rc1.use_count() == 2);
        assert(rc2.use_count() == 2);
        assert(rc2->value == 42);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Assignment working correctly" << std::endl;
}

//=============================================================================
// Test: Self Assignment
//=============================================================================

TEST(self_assignment) {
    std::cout << "  Testing self assignment..." << std::endl;
    
    {
        Rc<TestObject> rc(new TestObject(42));
        rc = rc;  // Self assignment
        
        assert(rc);
        assert(rc->value == 42);
        assert(rc.use_count() == 1);
        assert(object_count.load() == 1);
    }
    
    assert(object_count.load() == 0);
    
    std::cout << "  Self assignment working correctly" << std::endl;
}

//=============================================================================
// Test: Null Rc
//=============================================================================

TEST(null_rc) {
    std::cout << "  Testing null Rc..." << std::endl;
    
    Rc<TestObject> rc;
    assert(!rc);
    assert(rc.use_count() == 0);
    assert(rc.get() == nullptr);
    
    Rc<TestObject> rc2(static_cast<TestObject*>(nullptr));
    assert(!rc2);
    assert(rc2.use_count() == 0);
    
    std::cout << "  Null Rc working correctly" << std::endl;
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Reference Counting Tests\n";
    std::cout << "================================================================================\n\n";
    
    // Run all tests
    run_test_rc_basic_creation();
    run_test_rc_copy();
    run_test_rc_move();
    run_test_rc_clone();
    run_test_rc_reset();
    run_test_rc_make_factory();
    run_test_arc_basic();
    run_test_arc_thread_safety();
    run_test_weak_reference();
    run_test_weak_with_arc();
    run_test_cycle_breaking();
    run_test_multiple_references();
    run_test_assignment();
    run_test_self_assignment();
    run_test_null_rc();
    
    // Print summary
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Reference counting is working correctly! 🔄\n";
    } else {
        std::cout << "\n✗ Some tests failed. Please review the output above.\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
