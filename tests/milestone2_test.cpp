#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <chrono>

// Include Milestone 2 libraries
#include "stdlib/collections/hashmap.h"
#include "stdlib/json/json.h"
#include "stdlib/random/random.h"

using namespace sapphire::stdlib;

void test_hashmap() {
    std::cout << "Testing Hash Maps..." << std::endl;
    
    // Test creation
    auto* map = Collections::create_hashmap();
    assert(map != nullptr);
    assert(Collections::hashmap_empty(map));
    assert(Collections::hashmap_size(map) == 0);
    
    // Test integer operations
    assert(Collections::hashmap_set_int(map, "age", 25));
    assert(Collections::hashmap_set_int(map, "score", 95));
    assert(Collections::hashmap_set_int(map, "level", 10));
    
    assert(Collections::hashmap_size(map) == 3);
    assert(!Collections::hashmap_empty(map));
    
    assert(Collections::hashmap_get_int(map, "age") == 25);
    assert(Collections::hashmap_get_int(map, "score") == 95);
    assert(Collections::hashmap_get_int(map, "level") == 10);
    
    assert(Collections::hashmap_contains(map, "age"));
    assert(!Collections::hashmap_contains(map, "missing"));
    
    // Test double operations
    assert(Collections::hashmap_set_double(map, "pi", 3.14159));
    assert(std::abs(Collections::hashmap_get_double(map, "pi") - 3.14159) < 0.0001);
    
    // Test string operations
    assert(Collections::hashmap_set_string(map, "name", "Alice"));
    assert(Collections::hashmap_get_string(map, "name") == "Alice");
    
    // Test removal
    assert(Collections::hashmap_remove(map, "level"));
    assert(!Collections::hashmap_contains(map, "level"));
    assert(Collections::hashmap_size(map) == 4);
    
    // Test overwrite
    assert(Collections::hashmap_set_int(map, "age", 26));
    assert(Collections::hashmap_get_int(map, "age") == 26);
    assert(Collections::hashmap_size(map) == 4); // Size shouldn't change
    
    Collections::destroy_hashmap(map);
    
    std::cout << "✓ Hash Map tests passed" << std::endl;
}

void test_json() {
    std::cout << "Testing JSON..." << std::endl;
    
    // Test parsing simple JSON
    std::string json_text = R"({
        "name": "Alice",
        "age": 25,
        "active": true,
        "score": 95.5,
        "tags": ["developer", "sapphire"],
        "address": {
            "city": "New York",
            "zip": 10001
        }
    })";
    
    auto json_value = JSON::parse_json(json_text);
    assert(json_value != nullptr);
    assert(json_value->is_object());
    
    const auto& obj = json_value->as_object();
    
    // Test string access
    assert(obj.at("name")->is_string());
    assert(obj.at("name")->as_string() == "Alice");
    
    // Test number access
    assert(obj.at("age")->is_number());
    assert(obj.at("age")->as_number() == 25);
    
    // Test boolean access
    assert(obj.at("active")->is_bool());
    assert(obj.at("active")->as_bool() == true);
    
    // Test array access
    assert(obj.at("tags")->is_array());
    const auto& tags = obj.at("tags")->as_array();
    assert(tags.size() == 2);
    assert(tags[0]->as_string() == "developer");
    assert(tags[1]->as_string() == "sapphire");
    
    // Test nested object
    assert(obj.at("address")->is_object());
    const auto& address = obj.at("address")->as_object();
    assert(address.at("city")->as_string() == "New York");
    assert(address.at("zip")->as_number() == 10001);
    
    // Test JSON generation
    std::string generated = JSON::stringify_json(json_value, false);
    assert(!generated.empty());
    
    // Test creating JSON programmatically
    auto new_obj = JSON::create_json_object();
    auto& new_obj_data = new_obj->as_object();
    new_obj_data["test"] = JSON::create_json_string("value");
    new_obj_data["number"] = JSON::create_json_number(42);
    
    std::string simple_json = JSON::stringify_json(new_obj, false);
    assert(simple_json.find("test") != std::string::npos);
    assert(simple_json.find("42") != std::string::npos);
    
    std::cout << "✓ JSON tests passed" << std::endl;
}

void test_random() {
    std::cout << "Testing Random Numbers..." << std::endl;
    
    // Test seeding
    Random::random_seed(12345);
    
    // Test uniform distribution
    double uniform1 = Random::random_uniform();
    double uniform2 = Random::random_uniform();
    assert(uniform1 >= 0.0 && uniform1 < 1.0);
    assert(uniform2 >= 0.0 && uniform2 < 1.0);
    assert(uniform1 != uniform2); // Very unlikely to be equal
    
    // Test uniform range
    double range_val = Random::random_uniform_range(10.0, 20.0);
    assert(range_val >= 10.0 && range_val <= 20.0);
    
    // Test integer random
    int64_t int_val = Random::random_int(1, 10);
    assert(int_val >= 1 && int_val <= 10);
    
    // Test normal distribution
    double normal_val = Random::random_normal();
    // Normal values can be any real number, just check it's finite
    assert(std::isfinite(normal_val));
    
    // Test normal with parameters
    double normal_param = Random::random_normal_params(100.0, 15.0);
    assert(std::isfinite(normal_param));
    
    // Test boolean
    bool bool_val = Random::random_boolean(0.5);
    // Can be either true or false
    
    // Test reproducibility with same seed
    Random::random_seed(54321);
    double val1 = Random::random_uniform();
    Random::random_seed(54321);
    double val2 = Random::random_uniform();
    assert(std::abs(val1 - val2) < 0.0001); // Should be identical
    
    // Test cryptographic random
    uint64_t crypto1 = Random::CryptoRandom::uint64();
    uint64_t crypto2 = Random::CryptoRandom::uint64();
    assert(crypto1 != crypto2); // Very unlikely to be equal
    
    double crypto_uniform = Random::CryptoRandom::uniform();
    assert(crypto_uniform >= 0.0 && crypto_uniform < 1.0);
    
    std::cout << "✓ Random Number tests passed" << std::endl;
}

void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    // Test HashMap C API
    void* map = sapphire_hashmap_create();
    assert(map != nullptr);
    
    assert(sapphire_hashmap_set_int(map, "test", 123));
    assert(sapphire_hashmap_get_int(map, "test") == 123);
    assert(sapphire_hashmap_contains(map, "test"));
    assert(sapphire_hashmap_size(map) == 1);
    
    sapphire_hashmap_destroy(map);
    
    // Test JSON C API
    void* json_obj = sapphire_json_create_object();
    assert(json_obj != nullptr);
    
    void* json_str = sapphire_json_create_string("test_value");
    sapphire_json_object_set(json_obj, "key", json_str);
    
    void* retrieved = sapphire_json_object_get(json_obj, "key");
    assert(retrieved != nullptr);
    assert(sapphire_json_is_string(retrieved));
    
    sapphire_json_destroy(json_obj);
    sapphire_json_destroy(json_str);
    sapphire_json_destroy(retrieved);
    
    // Test Random C API
    sapphire_random_seed(98765);
    
    double uniform = sapphire_random_uniform();
    assert(uniform >= 0.0 && uniform < 1.0);
    
    int64_t random_int = sapphire_random_int(1, 100);
    assert(random_int >= 1 && random_int <= 100);
    
    std::cout << "✓ C API tests passed" << std::endl;
}

void test_performance() {
    std::cout << "Testing Performance..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test HashMap performance with 100K entries
    auto* map = Collections::create_hashmap();
    const int count = 100000;
    
    // Insert 100K entries
    for (int i = 0; i < count; i++) {
        std::string key = "key_" + std::to_string(i);
        Collections::hashmap_set_int(map, key, i * i);
    }
    
    assert(Collections::hashmap_size(map) == count);
    
    // Lookup all entries
    for (int i = 0; i < count; i++) {
        std::string key = "key_" + std::to_string(i);
        int64_t value = Collections::hashmap_get_int(map, key);
        assert(value == i * i);
    }
    
    Collections::destroy_hashmap(map);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  100K HashMap operations completed in " << duration.count() << " ms" << std::endl;
    
    // Performance should be reasonable (less than 1 second for 100K operations)
    assert(duration.count() < 1000);
    
    // Test JSON parsing performance
    start = std::chrono::high_resolution_clock::now();
    
    std::string large_json = R"({"data": [)";
    for (int i = 0; i < 1000; i++) {
        if (i > 0) large_json += ",";
        large_json += R"({"id": )" + std::to_string(i) + R"(, "value": "item_)" + std::to_string(i) + R"("})";
    }
    large_json += "]}";
    
    auto json_value = JSON::parse_json(large_json);
    assert(json_value != nullptr);
    assert(json_value->is_object());
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Large JSON parsing completed in " << duration.count() << " ms" << std::endl;
    
    // JSON parsing should be fast (less than 100ms for 1K objects)
    assert(duration.count() < 100);
    
    std::cout << "✓ Performance tests passed" << std::endl;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              MILESTONE 2: LIBRARY TESTS                     ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    try {
        test_hashmap();
        test_json();
        test_random();
        test_c_api();
        test_performance();
        
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ALL TESTS PASSED! ✅                     ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "🎯 Milestone 2 Libraries Ready for Integration!" << std::endl;
        std::cout << "✅ Hash Maps - Fast key-value storage" << std::endl;
        std::cout << "✅ JSON Support - Complete parsing and generation" << std::endl;
        std::cout << "✅ Random Numbers - Uniform, normal, and crypto random" << std::endl;
        std::cout << "✅ C API - Compiler integration ready" << std::endl;
        std::cout << "✅ Performance - 100K+ operations per second" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}