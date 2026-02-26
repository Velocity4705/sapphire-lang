#include "../../runtime/concurrency/thread.h"
#include <iostream>
#include <cassert>
#include <atomic>

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

TEST(thread_spawn_join) {
    bool executed = false;
    
    Thread t = Thread::spawn([&executed]() {
        executed = true;
    });
    
    t.join();
    assert(executed);
}

TEST(thread_with_args) {
    int result = 0;
    
    Thread t = Thread::spawn([](int* out, int a, int b) {
        *out = a + b;
    }, &result, 10, 20);
    
    t.join();
    assert(result == 30);
}

TEST(thread_multiple) {
    std::atomic<int> counter(0);
    
    Thread t1 = Thread::spawn([&counter]() {
        counter++;
    });
    
    Thread t2 = Thread::spawn([&counter]() {
        counter++;
    });
    
    Thread t3 = Thread::spawn([&counter]() {
        counter++;
    });
    
    t1.join();
    t2.join();
    t3.join();
    
    assert(counter == 3);
}

TEST(thread_sleep) {
    auto start = std::chrono::steady_clock::now();
    
    Thread t = Thread::spawn([]() {
        Thread::sleep(100);
    });
    
    t.join();
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    assert(duration >= 100);
}

TEST(thread_yield) {
    Thread t = Thread::spawn([]() {
        Thread::yield();
    });
    
    t.join();
}

TEST(thread_id) {
    Thread t = Thread::spawn([]() {});
    assert(t.id() > 0);
    t.join();
}

TEST(thread_name) {
    Thread t = Thread::spawn([]() {});
    t.set_name("test_thread");
    assert(t.name() == "test_thread");
    t.join();
}

TEST(thread_hardware_concurrency) {
    size_t cores = Thread::hardware_concurrency();
    assert(cores > 0);
}

TEST(thread_move) {
    Thread t1 = Thread::spawn([]() {
        Thread::sleep(10);
    });
    
    Thread t2 = std::move(t1);
    assert(!t1.joinable());
    assert(t2.joinable());
    
    t2.join();
}

TEST(thread_detach) {
    Thread t = Thread::spawn([]() {
        Thread::sleep(10);
    });
    
    t.detach();
    assert(!t.joinable());
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Thread Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_thread_spawn_join();
    run_test_thread_with_args();
    run_test_thread_multiple();
    run_test_thread_sleep();
    run_test_thread_yield();
    run_test_thread_id();
    run_test_thread_name();
    run_test_thread_hardware_concurrency();
    run_test_thread_move();
    run_test_thread_detach();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Thread system is working! ⚡\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
