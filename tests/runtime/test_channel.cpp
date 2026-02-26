#include "../../runtime/concurrency/channel.h"
#include "../../runtime/concurrency/thread.h"
#include <iostream>
#include <cassert>

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

TEST(channel_send_receive) {
    Channel<int> ch;
    
    Thread sender = Thread::spawn([&ch]() {
        ch.send(42);
    });
    
    int value = ch.receive();
    assert(value == 42);
    
    sender.join();
}

TEST(channel_multiple_messages) {
    Channel<int> ch(10);
    
    Thread sender = Thread::spawn([&ch]() {
        for (int i = 0; i < 5; i++) {
            ch.send(i);
        }
    });
    
    for (int i = 0; i < 5; i++) {
        int value = ch.receive();
        assert(value == i);
    }
    
    sender.join();
}

TEST(channel_try_send_receive) {
    Channel<int> ch(2);
    
    assert(ch.try_send(1));
    assert(ch.try_send(2));
    assert(!ch.try_send(3)); // Full
    
    auto v1 = ch.try_receive();
    assert(v1.has_value() && v1.value() == 1);
    
    auto v2 = ch.try_receive();
    assert(v2.has_value() && v2.value() == 2);
    
    auto v3 = ch.try_receive();
    assert(!v3.has_value()); // Empty
}

TEST(channel_close) {
    Channel<int> ch;
    ch.close();
    assert(ch.is_closed());
}

TEST(channel_buffered) {
    Channel<int> ch(3);
    
    ch.send(1);
    ch.send(2);
    ch.send(3);
    
    assert(ch.size() == 3);
    assert(ch.receive() == 1);
    assert(ch.receive() == 2);
    assert(ch.receive() == 3);
}

TEST(channel_move) {
    Channel<int> ch1;
    ch1.send(42);
    
    Channel<int> ch2 = std::move(ch1);
    assert(ch2.receive() == 42);
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Channel Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_channel_send_receive();
    run_test_channel_multiple_messages();
    run_test_channel_try_send_receive();
    run_test_channel_close();
    run_test_channel_buffered();
    run_test_channel_move();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Channel is working! 📡\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
