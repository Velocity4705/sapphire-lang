#include "../core/string.h"
#include "../core/vec.h"
#include "../core/hashmap.h"
#include "../io/file.h"
#include "../math/math.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace sapphire::stdlib;

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

//=============================================================================
// String Tests
//=============================================================================

TEST(string_basic) {
    String s1;
    assert(s1.empty());
    
    String s2("Hello");
    assert(s2.length() == 5);
    assert(s2 == String("Hello"));
    
    String s3 = s2;
    assert(s3 == s2);
}

TEST(string_concat) {
    String s1("Hello");
    String s2(" World");
    String s3 = s1 + s2;
    assert(s3 == String("Hello World"));
    
    s1 += s2;
    assert(s1 == String("Hello World"));
}

TEST(string_substring) {
    String s("Hello World");
    assert(s.substring(0, 5) == String("Hello"));
    assert(s.substring(6) == String("World"));
}

TEST(string_search) {
    String s("Hello World");
    assert(s.contains(String("World")));
    assert(!s.contains(String("Foo")));
    assert(s.index_of(String("World")) == 6);
    assert(s.starts_with(String("Hello")));
    assert(s.ends_with(String("World")));
}

TEST(string_case) {
    String s("Hello World");
    assert(s.to_upper() == String("HELLO WORLD"));
    assert(s.to_lower() == String("hello world"));
}

TEST(string_trim) {
    String s("  Hello  ");
    assert(s.trim() == String("Hello"));
    assert(s.trim_left() == String("Hello  "));
    assert(s.trim_right() == String("  Hello"));
}

TEST(string_split) {
    String s("a,b,c");
    auto parts = s.split(String(","));
    assert(parts.size() == 3);
    assert(parts[0] == String("a"));
    assert(parts[1] == String("b"));
    assert(parts[2] == String("c"));
}

TEST(string_join) {
    std::vector<String> parts = {String("a"), String("b"), String("c")};
    String joined = String::join(parts, String(","));
    assert(joined == String("a,b,c"));
}

TEST(string_conversion) {
    assert(String::from_int(42) == String("42"));
    assert(String::from_bool(true) == String("true"));
    assert(String("42").to_int() == 42);
    assert(std::abs(String("3.14").to_double() - 3.14) < 0.01);
}

//=============================================================================
// Vec Tests
//=============================================================================

TEST(vec_basic) {
    Vec<int> v;
    assert(v.empty());
    assert(v.size() == 0);
    
    v.push(1);
    v.push(2);
    v.push(3);
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);
}

TEST(vec_pop) {
    Vec<int> v;
    v.push(1);
    v.push(2);
    v.push(3);
    
    assert(v.pop() == 3);
    assert(v.size() == 2);
    assert(v.pop() == 2);
    assert(v.pop() == 1);
    assert(v.empty());
}

TEST(vec_insert_remove) {
    Vec<int> v;
    v.push(1);
    v.push(3);
    v.insert(1, 2);
    
    assert(v.size() == 3);
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 3);
    
    v.remove(1);
    assert(v.size() == 2);
    assert(v[0] == 1);
    assert(v[1] == 3);
}

TEST(vec_iteration) {
    Vec<int> v;
    v.push(1);
    v.push(2);
    v.push(3);
    
    int sum = 0;
    for (int x : v) {
        sum += x;
    }
    assert(sum == 6);
}

TEST(vec_copy) {
    Vec<int> v1;
    v1.push(1);
    v1.push(2);
    
    Vec<int> v2 = v1;
    assert(v2.size() == 2);
    assert(v2[0] == 1);
    assert(v2[1] == 2);
}

//=============================================================================
// HashMap Tests
//=============================================================================

TEST(hashmap_basic) {
    HashMap<int, String> map;
    assert(map.empty());
    
    map.insert(1, String("one"));
    map.insert(2, String("two"));
    map.insert(3, String("three"));
    
    assert(map.size() == 3);
    assert(*map.get(1) == String("one"));
    assert(*map.get(2) == String("two"));
    assert(*map.get(3) == String("three"));
}

TEST(hashmap_contains) {
    HashMap<int, String> map;
    map.insert(1, String("one"));
    
    assert(map.contains(1));
    assert(!map.contains(2));
}

TEST(hashmap_remove) {
    HashMap<int, String> map;
    map.insert(1, String("one"));
    map.insert(2, String("two"));
    
    map.remove(1);
    assert(!map.contains(1));
    assert(map.contains(2));
    assert(map.size() == 1);
}

TEST(hashmap_update) {
    HashMap<int, String> map;
    map.insert(1, String("one"));
    map.insert(1, String("ONE"));
    
    assert(map.size() == 1);
    assert(*map.get(1) == String("ONE"));
}

TEST(hashmap_operator) {
    HashMap<int, String> map;
    map[1] = String("one");
    map[2] = String("two");
    
    assert(map[1] == String("one"));
    assert(map[2] == String("two"));
}

//=============================================================================
// File Tests
//=============================================================================

TEST(file_write_read) {
    String path("test_file.txt");
    String content("Hello, File!");
    
    File::write_all(path, content);
    String read_content = File::read_all(path);
    
    assert(read_content == content);
    File::remove_file(path);
}

TEST(file_exists) {
    String path("test_exists.txt");
    
    assert(!File::exists(path));
    File::write_all(path, String("test"));
    assert(File::exists(path));
    File::remove_file(path);
    assert(!File::exists(path));
}

TEST(file_lines) {
    String path("test_lines.txt");
    File file = File::open(path, "w");
    
    file.write_line(String("Line 1"));
    file.write_line(String("Line 2"));
    file.write_line(String("Line 3"));
    file.close();
    
    file = File::open(path, "r");
    Vec<String> lines = file.read_lines();
    file.close();
    
    assert(lines.size() == 3);
    assert(lines[0] == String("Line 1"));
    assert(lines[1] == String("Line 2"));
    assert(lines[2] == String("Line 3"));
    
    File::remove_file(path);
}

//=============================================================================
// Math Tests
//=============================================================================

TEST(math_basic) {
    assert(Math::abs(-5.0) == 5.0);
    assert(Math::min(3.0, 5.0) == 3.0);
    assert(Math::max(3.0, 5.0) == 5.0);
    assert(Math::clamp(10.0, 0.0, 5.0) == 5.0);
}

TEST(math_trig) {
    assert(std::abs(Math::sin(0.0)) < 0.001);
    assert(std::abs(Math::cos(0.0) - 1.0) < 0.001);
    assert(std::abs(Math::sin(Math::PI / 2) - 1.0) < 0.001);
}

TEST(math_exp) {
    assert(std::abs(Math::sqrt(16.0) - 4.0) < 0.001);
    assert(std::abs(Math::pow(2.0, 3.0) - 8.0) < 0.001);
    assert(std::abs(Math::exp(0.0) - 1.0) < 0.001);
    assert(std::abs(Math::log(Math::E) - 1.0) < 0.001);
}

TEST(math_rounding) {
    assert(Math::floor(3.7) == 3.0);
    assert(Math::ceil(3.2) == 4.0);
    assert(Math::round(3.5) == 4.0);
    assert(Math::trunc(3.7) == 3.0);
}

TEST(math_constants) {
    assert(Math::PI > 3.14 && Math::PI < 3.15);
    assert(Math::E > 2.71 && Math::E < 2.72);
    assert(Math::TAU > 6.28 && Math::TAU < 6.29);
}

//=============================================================================
// Main
//=============================================================================

int main() {
    std::cout << "================================================================================\n";
    std::cout << "Sapphire Standard Library Tests\n";
    std::cout << "================================================================================\n\n";
    
    // String tests
    std::cout << "String Tests:\n";
    run_test_string_basic();
    run_test_string_concat();
    run_test_string_substring();
    run_test_string_search();
    run_test_string_case();
    run_test_string_trim();
    run_test_string_split();
    run_test_string_join();
    run_test_string_conversion();
    
    // Vec tests
    std::cout << "\nVec Tests:\n";
    run_test_vec_basic();
    run_test_vec_pop();
    run_test_vec_insert_remove();
    run_test_vec_iteration();
    run_test_vec_copy();
    
    // HashMap tests
    std::cout << "\nHashMap Tests:\n";
    run_test_hashmap_basic();
    run_test_hashmap_contains();
    run_test_hashmap_remove();
    run_test_hashmap_update();
    run_test_hashmap_operator();
    
    // File tests
    std::cout << "\nFile Tests:\n";
    run_test_file_write_read();
    run_test_file_exists();
    run_test_file_lines();
    
    // Math tests
    std::cout << "\nMath Tests:\n";
    run_test_math_basic();
    run_test_math_trig();
    run_test_math_exp();
    run_test_math_rounding();
    run_test_math_constants();
    
    // Summary
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! Standard library is working correctly! 📚\n";
    } else {
        std::cout << "\n✗ Some tests failed. Please review the output above.\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
