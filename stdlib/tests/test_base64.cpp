#include "../encoding/base64.h"
#include <iostream>
#include <cassert>

using namespace sapphire::base64;

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

TEST(base64_encode_simple) {
    std::string encoded = encode("Hello");
    assert(encoded == "SGVsbG8=");
}

TEST(base64_decode_simple) {
    std::string decoded = decode("SGVsbG8=");
    assert(decoded == "Hello");
}

TEST(base64_roundtrip) {
    std::string original = "The quick brown fox jumps over the lazy dog";
    std::string encoded = encode(original);
    std::string decoded = decode(encoded);
    assert(decoded == original);
}

TEST(base64_empty) {
    std::string encoded = encode("");
    assert(encoded == "");
    std::string decoded = decode("");
    assert(decoded == "");
}

TEST(base64_special_chars) {
    std::string original = "Hello, World! 123 @#$%";
    std::string encoded = encode(original);
    std::string decoded = decode(encoded);
    assert(decoded == original);
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Base64 Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_base64_encode_simple();
    run_test_base64_decode_simple();
    run_test_base64_roundtrip();
    run_test_base64_empty();
    run_test_base64_special_chars();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Base64 is working! 🔐\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
