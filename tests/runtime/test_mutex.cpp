#include "../../runtime/concurrency/mutex.h"
#include "../../runtime/concurrency/thread.h"
#include <iostream>
#include <cassert>
#include <atomic>
#include <vector>

using namespace sapphire::runtime;

int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) \
    void test_##name(); \
    void run_test_##name() { \
        std::cout << "Running test: " #name "..." << std::endl; \
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

TEST(mutex_basic) {
    Mutex m;
    m.lock();
    m.unlock();
}

TEST(mutex_try_lock) {
    Mutex m;
    assert(m.try_lock());
    assert(!m.try_lock());
    m.unlock();
    assert(m.try_lock());
    m.unlock();
}

TEST(mutex_guard) {
    Mutex m;
    {
        auto guard = m.lock_guard();
        // Mutex is locked here
    }
    // Mutex is unlocked here
    assert(m.try_lock());
    m.unlock();
}

TEST(mutex_concurrent_increment) {
    Mutex m;
    int counter = 0;
    const int num_threads = 10;
    const int increments_per_thread = 1000;
    
    std::vector<Thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.push_back(Thread::spawn([&m, &counter, increments_per_thread]() {
            for (int j = 0; j < increments_per_thread; j++) {
                auto guard = m.lock_guard();
                counter++;
            }
        }));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(counter == num_threads * increments_per_thread);
}

TEST(mutex_protects_shared_data) {
    Mutex m;
    std::vector<int> data;
    
    Thread t1 = Thread::spawn([&m, &data]() {
        for (int i = 0; i < 100; i++) {
            auto guard = m.lock_guard();
            data.push_back(i);
        }
    });
    
    Thread t2 = Thread::spawn([&m, &data]() {
        for (int i = 100; i < 200; i++) {
            auto guard = m.lock_guard();
            data.push_back(i);
        }
    });
    
    t1.join();
    t2.join();
    
    assert(data.size() == 200);
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Mutex Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_mutex_basic();
    run_test_mutex_try_lock();
    run_test_mutex_guard();
    run_test_mutex_concurrent_increment();
    run_test_mutex_protects_shared_data();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Mutex is working! 🔒\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
