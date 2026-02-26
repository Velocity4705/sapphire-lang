#include "../../runtime/concurrency/rwlock.h"
#include "../../runtime/concurrency/thread.h"
#include <iostream>
#include <cassert>
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

TEST(rwlock_read_lock) {
    RwLock lock;
    lock.read_lock();
    lock.read_unlock();
}

TEST(rwlock_write_lock) {
    RwLock lock;
    lock.write_lock();
    lock.write_unlock();
}

TEST(rwlock_read_guard) {
    RwLock lock;
    {
        auto guard = lock.read_guard();
    }
    assert(lock.try_write_lock());
    lock.write_unlock();
}

TEST(rwlock_write_guard) {
    RwLock lock;
    {
        auto guard = lock.write_guard();
    }
    assert(lock.try_read_lock());
    lock.read_unlock();
}

TEST(rwlock_multiple_readers) {
    RwLock lock;
    int data = 0;
    std::vector<Thread> readers;
    
    for (int i = 0; i < 5; i++) {
        readers.push_back(Thread::spawn([&lock, &data]() {
            auto guard = lock.read_guard();
            Thread::sleep(10);
            (void)data;
        }));
    }
    
    for (auto& t : readers) {
        t.join();
    }
}

TEST(rwlock_writer_excludes_readers) {
    RwLock lock;
    int data = 0;
    bool writer_done = false;
    
    Thread writer = Thread::spawn([&lock, &data, &writer_done]() {
        auto guard = lock.write_guard();
        data = 42;
        Thread::sleep(50);
        writer_done = true;
    });
    
    Thread::sleep(10);
    
    Thread reader = Thread::spawn([&lock, &data, &writer_done]() {
        auto guard = lock.read_guard();
        assert(writer_done);
        assert(data == 42);
    });
    
    writer.join();
    reader.join();
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "RwLock Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_rwlock_read_lock();
    run_test_rwlock_write_lock();
    run_test_rwlock_read_guard();
    run_test_rwlock_write_guard();
    run_test_rwlock_multiple_readers();
    run_test_rwlock_writer_excludes_readers();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! RwLock is working! 📖\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
