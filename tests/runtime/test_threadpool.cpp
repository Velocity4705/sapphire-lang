#include "../../runtime/concurrency/threadpool.h"
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

TEST(threadpool_basic) {
    ThreadPool pool(4);
    assert(pool.num_threads() == 4);
}

TEST(threadpool_submit_task) {
    ThreadPool pool(2);
    
    auto future = pool.submit([]() {
        return 42;
    });
    
    assert(future.get() == 42);
}

TEST(threadpool_multiple_tasks) {
    ThreadPool pool(4);
    std::atomic<int> counter(0);
    
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; i++) {
        futures.push_back(pool.submit([&counter]() {
            counter++;
        }));
    }
    
    for (auto& f : futures) {
        f.get();
    }
    
    assert(counter == 10);
}

TEST(threadpool_with_args) {
    ThreadPool pool(2);
    
    auto future = pool.submit([](int a, int b) {
        return a + b;
    }, 10, 20);
    
    assert(future.get() == 30);
}

TEST(threadpool_wait_all) {
    ThreadPool pool(4);
    std::atomic<int> counter(0);
    
    for (int i = 0; i < 20; i++) {
        pool.submit([&counter]() {
            counter++;
        });
    }
    
    pool.wait_all();
    assert(counter == 20);
}

TEST(threadpool_parallel_sum) {
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;
    
    for (int i = 0; i < 10; i++) {
        futures.push_back(pool.submit([i]() {
            return i * i;
        }));
    }
    
    int sum = 0;
    for (auto& f : futures) {
        sum += f.get();
    }
    
    assert(sum == 285); // 0 + 1 + 4 + 9 + ... + 81
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "ThreadPool Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_threadpool_basic();
    run_test_threadpool_submit_task();
    run_test_threadpool_multiple_tasks();
    run_test_threadpool_with_args();
    run_test_threadpool_wait_all();
    run_test_threadpool_parallel_sum();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! ThreadPool is working! 🏊\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
