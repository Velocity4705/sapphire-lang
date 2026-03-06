#include "../utils/utils.h"
#include <iostream>
#include <cassert>

using namespace sapphire::stdlib;

void test_string_conversion() {
    std::cout << "Testing string conversion..." << std::endl;
    
    assert(Utils::to_string(42) == "42");
    assert(Utils::to_string(3.14) == "3.140000");
    assert(Utils::to_string(true) == "true");
    assert(Utils::to_string(false) == "false");
    
    assert(Utils::to_int("123") == 123);
    assert(Utils::to_double("3.14") == 3.14);
    assert(Utils::to_bool("true") == true);
    assert(Utils::to_bool("false") == false);
    
    std::cout << "  ✓ String conversion functions" << std::endl;
}

void test_case_conversion() {
    std::cout << "Testing case conversion..." << std::endl;
    
    assert(Utils::to_upper("hello") == "HELLO");
    assert(Utils::to_lower("WORLD") == "world");
    assert(Utils::to_upper("MiXeD") == "MIXED");
    
    std::cout << "  ✓ Case conversion functions" << std::endl;
}

void test_trimming() {
    std::cout << "Testing trimming..." << std::endl;
    
    assert(Utils::trim("  hello  ") == "hello");
    assert(Utils::ltrim("  hello") == "hello");
    assert(Utils::rtrim("hello  ") == "hello");
    assert(Utils::trim("\t\nhello\r\n") == "hello");
    
    std::cout << "  ✓ Trimming functions" << std::endl;
}

void test_split_join() {
    std::cout << "Testing split and join..." << std::endl;
    
    auto parts = Utils::split("a,b,c", ",");
    assert(parts.size() == 3);
    assert(parts[0] == "a");
    assert(parts[1] == "b");
    assert(parts[2] == "c");
    
    std::string joined = Utils::join(parts, "-");
    assert(joined == "a-b-c");
    
    std::cout << "  ✓ Split and join functions" << std::endl;
}

void test_string_predicates() {
    std::cout << "Testing string predicates..." << std::endl;
    
    assert(Utils::starts_with("hello world", "hello"));
    assert(!Utils::starts_with("hello world", "world"));
    
    assert(Utils::ends_with("hello world", "world"));
    assert(!Utils::ends_with("hello world", "hello"));
    
    assert(Utils::contains("hello world", "lo wo"));
    assert(!Utils::contains("hello world", "xyz"));
    
    std::cout << "  ✓ String predicate functions" << std::endl;
}

void test_string_manipulation() {
    std::cout << "Testing string manipulation..." << std::endl;
    
    assert(Utils::replace("hello world", "world", "universe") == "hello universe");
    assert(Utils::repeat("ab", 3) == "ababab");
    
    std::cout << "  ✓ String manipulation functions" << std::endl;
}

void test_character_classification() {
    std::cout << "Testing character classification..." << std::endl;
    
    assert(Utils::is_digit('5'));
    assert(!Utils::is_digit('a'));
    
    assert(Utils::is_alpha('a'));
    assert(!Utils::is_alpha('5'));
    
    assert(Utils::is_alnum('a'));
    assert(Utils::is_alnum('5'));
    assert(!Utils::is_alnum(' '));
    
    assert(Utils::is_whitespace(' '));
    assert(!Utils::is_whitespace('a'));
    
    std::cout << "  ✓ Character classification functions" << std::endl;
}

void test_math_utilities() {
    std::cout << "Testing math utilities..." << std::endl;
    
    assert(Utils::abs(-5) == 5);
    assert(Utils::abs(5) == 5);
    assert(Utils::abs(-3.14) == 3.14);
    
    assert(Utils::min(3, 5) == 3);
    assert(Utils::max(3, 5) == 5);
    assert(Utils::min(2.5, 3.5) == 2.5);
    assert(Utils::max(2.5, 3.5) == 3.5);
    
    std::cout << "  ✓ Math utility functions" << std::endl;
}

void test_random() {
    std::cout << "Testing random generation..." << std::endl;
    
    int r1 = Utils::random_int(1, 10);
    assert(r1 >= 1 && r1 <= 10);
    
    double r2 = Utils::random_double(0.0, 1.0);
    assert(r2 >= 0.0 && r2 <= 1.0);
    
    std::cout << "  ✓ Random generation functions" << std::endl;
}

void test_hash() {
    std::cout << "Testing hash functions..." << std::endl;
    
    uint32_t h1 = Utils::hash_string("hello");
    uint32_t h2 = Utils::hash_string("hello");
    uint32_t h3 = Utils::hash_string("world");
    
    assert(h1 == h2);
    assert(h1 != h3);
    
    uint32_t h4 = Utils::hash_int(42);
    assert(h4 != 0);
    
    std::cout << "  ✓ Hash functions" << std::endl;
}

void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    const char* upper = sapphire_utils_to_upper("hello");
    assert(std::string(upper) == "HELLO");
    
    const char* lower = sapphire_utils_to_lower("WORLD");
    assert(std::string(lower) == "world");
    
    assert(sapphire_utils_to_int("123") == 123);
    assert(sapphire_utils_to_double("3.14") == 3.14);
    
    assert(sapphire_utils_starts_with("hello", "hel"));
    assert(sapphire_utils_ends_with("hello", "llo"));
    
    int r = sapphire_utils_random_int(1, 100);
    assert(r >= 1 && r <= 100);
    
    std::cout << "  ✓ C API functions" << std::endl;
}

int main() {
    std::cout << "=== Utils Library Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_string_conversion();
    test_case_conversion();
    test_trimming();
    test_split_join();
    test_string_predicates();
    test_string_manipulation();
    test_character_classification();
    test_math_utilities();
    test_random();
    test_hash();
    test_c_api();
    
    std::cout << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    std::cout << "All Utils tests passed! 🛠️✅" << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    
    return 0;
}
