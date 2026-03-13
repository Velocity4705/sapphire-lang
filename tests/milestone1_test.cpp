#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <chrono>

// Include Milestone 1 libraries
#include "stdlib/collections/array.h"
#include "stdlib/math/math.h"
#include "stdlib/io/csv.h"

using namespace sapphire::stdlib;

void test_dynamic_arrays() {
    std::cout << "Testing Dynamic Arrays..." << std::endl;
    
    // Test integer arrays
    auto* int_array = Collections::create_array(sizeof(int64_t));
    assert(int_array != nullptr);
    assert(Collections::array_empty(int_array));
    
    // Test push operations
    Collections::array_push_int(int_array, 10);
    Collections::array_push_int(int_array, 20);
    Collections::array_push_int(int_array, 30);
    
    assert(Collections::array_size(int_array) == 3);
    assert(!Collections::array_empty(int_array));
    
    // Test get operations
    assert(Collections::array_get_int(int_array, 0) == 10);
    assert(Collections::array_get_int(int_array, 1) == 20);
    assert(Collections::array_get_int(int_array, 2) == 30);
    
    // Test set operations
    Collections::array_set_int(int_array, 1, 25);
    assert(Collections::array_get_int(int_array, 1) == 25);
    
    Collections::destroy_array(int_array);
    
    // Test double arrays
    auto* double_array = Collections::create_array(sizeof(double));
    Collections::array_push_double(double_array, 3.14);
    Collections::array_push_double(double_array, 2.71);
    
    assert(Collections::array_size(double_array) == 2);
    assert(std::abs(Collections::array_get_double(double_array, 0) - 3.14) < 0.001);
    assert(std::abs(Collections::array_get_double(double_array, 1) - 2.71) < 0.001);
    
    Collections::destroy_array(double_array);
    
    // Test string arrays
    auto* string_array = Collections::create_array(sizeof(char*));
    Collections::array_push_string(string_array, "Hello");
    Collections::array_push_string(string_array, "World");
    
    assert(Collections::array_size(string_array) == 2);
    assert(std::string(Collections::array_get_string(string_array, 0)) == "Hello");
    assert(std::string(Collections::array_get_string(string_array, 1)) == "World");
    
    Collections::destroy_array(string_array);
    
    std::cout << "✓ Dynamic Arrays tests passed" << std::endl;
}
void test_math_library() {
    std::cout << "Testing Math Library..." << std::endl;
    
    // Test basic operations
    assert(std::abs(Math::abs(-5.5) - 5.5) < 0.001);
    assert(Math::min(3.0, 7.0) == 3.0);
    assert(Math::max(3.0, 7.0) == 7.0);
    
    // Test trigonometry
    double pi_4 = Math::PI / 4.0;
    assert(std::abs(Math::sin(pi_4) - 0.7071) < 0.001);
    assert(std::abs(Math::cos(pi_4) - 0.7071) < 0.001);
    assert(std::abs(Math::tan(pi_4) - 1.0) < 0.001);
    
    // Test exponential and logarithmic
    assert(std::abs(Math::exp(1.0) - Math::E) < 0.001);
    assert(std::abs(Math::log(Math::E) - 1.0) < 0.001);
    assert(std::abs(Math::log10(100.0) - 2.0) < 0.001);
    assert(std::abs(Math::log2(8.0) - 3.0) < 0.001);
    
    // Test power and square root
    assert(std::abs(Math::pow(2.0, 8.0) - 256.0) < 0.001);
    assert(std::abs(Math::sqrt(16.0) - 4.0) < 0.001);
    
    // Test rounding
    assert(Math::floor(3.7) == 3.0);
    assert(Math::ceil(3.2) == 4.0);
    assert(Math::round(3.7) == 4.0);
    assert(Math::round(3.2) == 3.0);
    
    // Test constants
    assert(std::abs(Math::PI - 3.14159) < 0.001);
    assert(std::abs(Math::E - 2.71828) < 0.001);
    
    std::cout << "✓ Math Library tests passed" << std::endl;
}

void test_csv_library() {
    std::cout << "Testing CSV Library..." << std::endl;
    
    // Create CSV data
    auto* csv_data = IO::create_csv_data();
    assert(csv_data != nullptr);
    
    // Test empty CSV
    assert(csv_data->row_count() == 0);
    assert(csv_data->col_count() == 0);
    
    // Add headers
    csv_data->headers = {"Name", "Age", "Score"};
    csv_data->has_headers = true;
    
    // Add rows
    csv_data->add_row({"Alice", "25", "95.5"});
    csv_data->add_row({"Bob", "30", "87.2"});
    csv_data->add_row({"Charlie", "28", "92.8"});
    
    assert(csv_data->row_count() == 3);
    assert(csv_data->col_count() == 3);
    
    // Test data access
    assert(csv_data->get(0, 0) == "Alice");
    assert(csv_data->get(0, 1) == "25");
    assert(csv_data->get(0, 2) == "95.5");
    
    assert(csv_data->get(1, 0) == "Bob");
    assert(csv_data->get(2, 0) == "Charlie");
    
    // Test set operation
    csv_data->set(1, 2, "88.0");
    assert(csv_data->get(1, 2) == "88.0");
    
    IO::destroy_csv_data(csv_data);
    
    std::cout << "✓ CSV Library tests passed" << std::endl;
}
void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    // Test array C API
    void* array = sapphire_array_create();
    assert(array != nullptr);
    
    sapphire_array_push_int(array, 100);
    sapphire_array_push_int(array, 200);
    sapphire_array_push_int(array, 300);
    
    assert(sapphire_array_size(array) == 3);
    assert(sapphire_array_get_int(array, 0) == 100);
    assert(sapphire_array_get_int(array, 1) == 200);
    assert(sapphire_array_get_int(array, 2) == 300);
    
    sapphire_array_destroy(array);
    
    // Test math C API
    assert(std::abs(sapphire_math_sqrt(16.0) - 4.0) < 0.001);
    assert(std::abs(sapphire_math_sin(sapphire_math_pi() / 2.0) - 1.0) < 0.001);
    assert(std::abs(sapphire_math_exp(1.0) - sapphire_math_e()) < 0.001);
    
    // Test CSV C API
    void* csv = sapphire_csv_create();
    assert(csv != nullptr);
    
    sapphire_csv_set_header(csv, 0, "Name");
    sapphire_csv_set_header(csv, 1, "Value");
    
    sapphire_csv_add_row(csv);
    sapphire_csv_set(csv, 0, 0, "Test");
    sapphire_csv_set(csv, 0, 1, "123");
    
    assert(sapphire_csv_row_count(csv) == 1);
    assert(sapphire_csv_col_count(csv) == 2);
    assert(std::string(sapphire_csv_get(csv, 0, 0)) == "Test");
    assert(std::string(sapphire_csv_get(csv, 0, 1)) == "123");
    
    sapphire_csv_destroy(csv);
    
    std::cout << "✓ C API tests passed" << std::endl;
}

void test_performance() {
    std::cout << "Testing Performance..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test large array operations
    auto* large_array = Collections::create_array(sizeof(int64_t));
    const int count = 100000;
    
    // Push 100K elements
    for (int i = 0; i < count; i++) {
        Collections::array_push_int(large_array, i * i);
    }
    
    assert(Collections::array_size(large_array) == count);
    
    // Access elements
    int64_t sum = 0;
    for (int i = 0; i < count; i++) {
        sum += Collections::array_get_int(large_array, i);
    }
    
    Collections::destroy_array(large_array);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "  100K array operations completed in " << duration.count() << " microseconds" << std::endl;
    std::cout << "  Operations per second: " << (static_cast<long long>(count) * 2 * 1000000) / duration.count() << std::endl;
    
    // Performance should be reasonable (less than 100ms for 100K operations)
    assert(duration.count() < 100000);
    
    std::cout << "✓ Performance tests passed" << std::endl;
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              MILESTONE 1: LIBRARY TESTS                     ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    try {
        test_dynamic_arrays();
        test_math_library();
        test_csv_library();
        test_c_api();
        test_performance();
        
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ALL TESTS PASSED! ✅                     ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "🎯 Milestone 1 Libraries Ready for Integration!" << std::endl;
        std::cout << "✅ Dynamic Arrays - Variable-size data structures" << std::endl;
        std::cout << "✅ Math Library - Complete mathematical functions" << std::endl;
        std::cout << "✅ CSV I/O - Structured data processing" << std::endl;
        std::cout << "✅ C API - Compiler integration ready" << std::endl;
        std::cout << "✅ Performance - 100K+ operations per second" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}