#include "../json/json.h"
#include <iostream>
#include <cassert>

using namespace sapphire::json;

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

TEST(json_null) {
    Value v;
    assert(v.is_null());
}

TEST(json_bool) {
    Value v(true);
    assert(v.is_bool());
    assert(v.as_bool() == true);
}

TEST(json_number) {
    Value v(42.5);
    assert(v.is_number());
    assert(v.as_number() == 42.5);
}

TEST(json_string) {
    Value v("hello");
    assert(v.is_string());
    assert(v.as_string() == "hello");
}

TEST(json_array) {
    Value v;
    v.push(Value(1));
    v.push(Value(2));
    v.push(Value(3));
    
    assert(v.is_array());
    assert(v.size() == 3);
    assert(v[0].as_number() == 1);
    assert(v[1].as_number() == 2);
    assert(v[2].as_number() == 3);
}

TEST(json_object) {
    Value v;
    v["name"] = Value("Alice");
    v["age"] = Value(30);
    
    assert(v.is_object());
    assert(v.has("name"));
    assert(v["name"].as_string() == "Alice");
    assert(v["age"].as_number() == 30);
}

TEST(json_parse_simple) {
    auto v = Value::parse(R"({"name": "Bob", "age": 25})");
    assert(v.is_object());
    assert(v["name"].as_string() == "Bob");
    assert(v["age"].as_number() == 25);
}

TEST(json_parse_array) {
    auto v = Value::parse(R"([1, 2, 3, 4, 5])");
    assert(v.is_array());
    assert(v.size() == 5);
    assert(v[0].as_number() == 1);
    assert(v[4].as_number() == 5);
}

TEST(json_parse_nested) {
    auto v = Value::parse(R"({"user": {"name": "Alice", "age": 30}})");
    assert(v["user"]["name"].as_string() == "Alice");
    assert(v["user"]["age"].as_number() == 30);
}

TEST(json_serialize) {
    Value v;
    v["name"] = Value("Charlie");
    v["age"] = Value(35);
    
    std::string json = v.serialize();
    assert(json.find("Charlie") != std::string::npos);
    assert(json.find("35") != std::string::npos);
}

int main() {
    std::cout << "================================================================================\n";
    std::cout << "JSON Tests\n";
    std::cout << "================================================================================\n\n";
    
    run_test_json_null();
    run_test_json_bool();
    run_test_json_number();
    run_test_json_string();
    run_test_json_array();
    run_test_json_object();
    run_test_json_parse_simple();
    run_test_json_parse_array();
    run_test_json_parse_nested();
    run_test_json_serialize();
    
    std::cout << "\n================================================================================\n";
    std::cout << "Test Summary\n";
    std::cout << "================================================================================\n";
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << tests_failed << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed! JSON library is working! 📄\n";
    }
    
    std::cout << "================================================================================\n";
    
    return tests_failed == 0 ? 0 : 1;
}
