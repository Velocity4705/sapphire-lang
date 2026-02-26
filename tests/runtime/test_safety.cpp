#include "runtime/safety.h"
#include "runtime/allocator.h"
#include <iostream>
#include <cassert>
#include <cstring>

using namespace sapphire::runtime;

// Test counters
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
// Test: Null Pointer Detection
//=============================================================================

TEST(null_pointer_detection) {
    std::cout << "  Testing null pointer detection..." << std::endl;
    
    // Null pointer should throw
    void* null_ptr = nullptr;
    EXPECT_THROW(SafetyChecker::check_null(null_ptr), NullPointerError);
    
    // Valid pointer should not throw
    int value = 42;
    void* valid_ptr = &value;
    EXPECT_NO_THROW(SafetyChecker::check_null(valid_ptr));
    
    std::cout << "  Null pointer detection working correctly" << std::endl;
}

//=============================================================================
// Test: Use-After-Free Detection
//=============================================================================

TEST(use_after_free_detection) {
    std::cout << "  Testing use-after-free detection..." << std::endl;
    
    // Allocate memory
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    
    // Should be valid initially
    EXPECT_NO_THROW(SafetyChecker::check_use_after_free(ptr));
    assert(!SafetyChecker::is_freed(ptr));
    
    // Mark as freed
    SafetyChecker::mark_freed(ptr);
    
    // Should detect use-after-free
    assert(SafetyChecker::is_freed(ptr));
    EXPECT_THROW(SafetyChecker::check_use_after_free(ptr), UseAfterFreeError);
    
    // Note: Don't call sapphire_free() after mark_freed() as it would be a double-free
    // In real usage, mark_freed() is called inside sapphire_free()
    
    std::cout << "  Use-after-free detection working correctly" << std::endl;
}

//=============================================================================
// Test: Double-Free Detection
//=============================================================================

TEST(double_free_detection) {
    std::cout << "  Testing double-free detection..." << std::endl;
    
    // Allocate memory
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    
    // First free should work
    EXPECT_NO_THROW(SafetyChecker::mark_freed(ptr));
    
    // Second free should throw
    EXPECT_THROW(SafetyChecker::mark_freed(ptr), DoubleFreeError);
    
    // Note: Don't call sapphire_free() after mark_freed()
    
    std::cout << "  Double-free detection working correctly" << std::endl;
}

//=============================================================================
// Test: Buffer Overflow Detection
//=============================================================================

TEST(buffer_overflow_detection) {
    std::cout << "  Testing buffer overflow detection..." << std::endl;
    
    size_t length = 10;
    
    // Valid indices should not throw
    EXPECT_NO_THROW(SafetyChecker::check_bounds(0, length));
    EXPECT_NO_THROW(SafetyChecker::check_bounds(5, length));
    EXPECT_NO_THROW(SafetyChecker::check_bounds(9, length));
    
    // Out of bounds should throw
    EXPECT_THROW(SafetyChecker::check_bounds(10, length), BufferOverflowError);
    EXPECT_THROW(SafetyChecker::check_bounds(100, length), BufferOverflowError);
    
    std::cout << "  Buffer overflow detection working correctly" << std::endl;
}

//=============================================================================
// Test: Type Mismatch Detection
//=============================================================================

TEST(type_mismatch_detection) {
    std::cout << "  Testing type mismatch detection..." << std::endl;
    
    // Allocate with type ID 1
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    
    // Same type should not throw
    EXPECT_NO_THROW(SafetyChecker::check_type(ptr, 1));
    
    // Different type should throw
    EXPECT_THROW(SafetyChecker::check_type(ptr, 2), TypeMismatchError);
    EXPECT_THROW(SafetyChecker::check_type(ptr, 999), TypeMismatchError);
    
    // Clean up
    sapphire_free(ptr);
    
    std::cout << "  Type mismatch detection working correctly" << std::endl;
}

//=============================================================================
// Test: Memory Poisoning
//=============================================================================

TEST(memory_poisoning) {
    std::cout << "  Testing memory poisoning..." << std::endl;
    
    // Allocate memory
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    
    // Write some data
    std::memset(ptr, 0xAA, 64);
    
    // Should not be poisoned initially
    assert(!SafetyChecker::is_poisoned(ptr, 64));
    
    // Test poison_memory directly
    SafetyChecker::poison_memory(ptr, 64);
    
    // Memory should be poisoned now
    assert(SafetyChecker::is_poisoned(ptr, 64));
    
    // Clean up
    sapphire_free(ptr);
    
    std::cout << "  Memory poisoning working correctly" << std::endl;
}

//=============================================================================
// Test: Pointer Validation
//=============================================================================

TEST(pointer_validation) {
    std::cout << "  Testing pointer validation..." << std::endl;
    
    // Null pointer is invalid
    assert(!SafetyChecker::is_valid_pointer(nullptr));
    
    // Low address is invalid
    assert(!SafetyChecker::is_valid_pointer(reinterpret_cast<void*>(0x100)));
    
    // Unaligned pointer is invalid
    assert(!SafetyChecker::is_valid_pointer(reinterpret_cast<void*>(0x1001)));
    
    // Valid heap pointer
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    assert(SafetyChecker::is_valid_pointer(ptr));
    
    // Clean up
    sapphire_free(ptr);
    
    std::cout << "  Pointer validation working correctly" << std::endl;
}

//=============================================================================
// Test: SafePtr Wrapper
//=============================================================================

TEST(safe_ptr_wrapper) {
    std::cout << "  Testing SafePtr wrapper..." << std::endl;
    
    // Allocate an integer
    int* raw_ptr = static_cast<int*>(sapphire_allocate(sizeof(int), 1));
    *raw_ptr = 42;
    
    // Wrap in SafePtr
    SafePtr<int> safe_ptr(raw_ptr);
    
    // Should be able to access
    assert(*safe_ptr == 42);
    assert(safe_ptr.get() == raw_ptr);
    
    // Clean up
    sapphire_free(raw_ptr);
    
    std::cout << "  SafePtr wrapper working correctly" << std::endl;
}

//=============================================================================
// Test: SafePtr with Null
//=============================================================================

TEST(safe_ptr_null) {
    std::cout << "  Testing SafePtr with null pointer..." << std::endl;
    
    // Null pointer should throw in constructor
    int* null_ptr = nullptr;
    EXPECT_THROW(SafePtr<int> safe_ptr(null_ptr), NullPointerError);
    
    std::cout << "  SafePtr null detection working correctly" << std::endl;
}

//=============================================================================
// Test: SafePtr with Freed Memory
//=============================================================================

TEST(safe_ptr_freed) {
    std::cout << "  Testing SafePtr with freed memory..." << std::endl;
    
    // Allocate and free
    int* ptr = static_cast<int*>(sapphire_allocate(sizeof(int), 1));
    SafetyChecker::mark_freed(ptr);
    
    // Should throw in constructor
    EXPECT_THROW(SafePtr<int> safe_ptr(ptr), UseAfterFreeError);
    
    // Note: Don't call sapphire_free() after mark_freed()
    
    std::cout << "  SafePtr freed memory detection working correctly" << std::endl;
}

//=============================================================================
// Test: C API
//=============================================================================

TEST(c_api) {
    std::cout << "  Testing C API..." << std::endl;
    
    // Allocate memory
    void* ptr = sapphire_allocate(64, 1);
    assert(ptr != nullptr);
    
    // Test is_valid_pointer
    assert(sapphire_is_valid_pointer(ptr));
    assert(!sapphire_is_valid_pointer(nullptr));
    
    // Test is_freed
    assert(!sapphire_is_freed(ptr));
    sapphire_mark_freed(ptr);
    assert(sapphire_is_freed(ptr));
    
    // Test check_bounds
    EXPECT_NO_THROW(sapphire_check_bounds(5, 10));
    EXPECT_THROW(sapphire_check_bounds(10, 10), BufferOverflowError);
    
    // Allocate another pointer for type checking
    void* ptr2 = sapphire_allocate(64, 1);
    
    // Test check_type
    EXPECT_NO_THROW(sapphire_check_type(ptr2, 1));
    EXPECT_THROW(sapphire_check_type(ptr2, 2), TypeMismatchError);
    
    // Test check_null
    EXPECT_NO_THROW(sapphire_check_null(ptr2));
    EXPECT_THROW(sapphire_check_null(nullptr), NullPointerError);
    
    // Clean up ptr2 only (ptr was already marked freed)
    sapphire_free(ptr2);
    
    std::cout << "  C API working correctly" << std::endl;
}

//=============================================================================
// Test: Error Messages
//=============================================================================

TEST(error_messages) {
    std::cout << "  Testing error messages..." << std::endl;
    
    // Test each error type has a clear message
    try {
        throw NullPointerError();
    } catch (const SafetyError& e) {
        std::string msg = e.what();
        assert(msg.find("Null pointer") != std::string::npos);
        std::cout << "    NullPointerError: " << msg << std::endl;
    }
    
    try {
        throw BufferOverflowError(10, 5);
    } catch (const SafetyError& e) {
        std::string msg = e.what();
        assert(msg.find("Buffer overflow") != std::string::npos);
        assert(msg.find("10") != std::string::npos);
        assert(msg.find("5") != std::string::npos);
        std::cout << "    BufferOverflowError: " << msg << std::endl;
    }
    
    try {
        throw TypeMismatchError(1, 2);
    } catch (const SafetyError& e) {
        std::string msg = e.what();
        assert(msg.find("Type mismatch") != std::string::npos);
        assert(msg.find("1") != std::string::npos);
        assert(msg.find("2") != std::string::npos);
        std::cout << "    TypeMismatchError: " << msg << std::endl;
    }
    
    void* test_ptr = reinterpret_cast<void*>(0x12345678);
    
    try {
        throw UseAfterFreeError(test_ptr);
    } catch (const SafetyError& e) {
        std::string msg = e.what();
        assert(msg.find("Use-after-free") != std::string::npos);
        std::cout << "    UseAfterFreeError: " << msg << std::endl;
    }
    
    try {
        throw DoubleFreeError(test_ptr);
    } catch (const SafetyError& e) {
        std::string msg = e.what();
        assert(msg.find("Double-free") != std::string::npos);
        std::cout << "    DoubleFreeError: " << msg << std::endl;
    }
    
    std::cout << "  All error messages are clear and informative" << std::endl;
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Memory Safety Tests\n";
    std::cout << "================================================================================\n\n";
    
    // Initialize allocator
    init_allocator();
    
    // Run all tests
    run_test_null_pointer_detection();
    run_test_use_after_free_detection();
    run_test_double_free_detection();
    run_test_buffer_overflow_detection();
    run_test_type_mismatch_detection();
    run_test_memory_poisoning();
    run_test_pointer_validation();
    run_test_safe_ptr_wrapper();
    run_test_safe_ptr_null();
    run_test_safe_ptr_freed();
    run_test_c_api();
    run_test_error_messages();
    
    // Print summary
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Memory safety is working correctly! 🛡️\n";
    } else {
        std::cout << "\n✗ Some tests failed. Please review the output above.\n";
    }
    
    std::cout << "================================================================================\n";
    
    // Cleanup
    shutdown_allocator();
    
    return tests_failed == 0 ? 0 : 1;
}
