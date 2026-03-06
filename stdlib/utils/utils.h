#ifndef SAPPHIRE_STDLIB_UTILS_H
#define SAPPHIRE_STDLIB_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace sapphire {
namespace stdlib {

/**
 * Utils - Utility Functions
 * 
 * Common utility functions for string manipulation, type conversion, and more.
 */

namespace Utils {
    // String utilities
    std::string to_string(int value);
    std::string to_string(double value);
    std::string to_string(bool value);
    
    int to_int(const std::string& str);
    double to_double(const std::string& str);
    bool to_bool(const std::string& str);
    
    std::string to_upper(const std::string& str);
    std::string to_lower(const std::string& str);
    std::string trim(const std::string& str);
    std::string ltrim(const std::string& str);
    std::string rtrim(const std::string& str);
    
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    
    bool starts_with(const std::string& str, const std::string& prefix);
    bool ends_with(const std::string& str, const std::string& suffix);
    bool contains(const std::string& str, const std::string& substring);
    
    std::string replace(const std::string& str, const std::string& from, const std::string& to);
    std::string repeat(const std::string& str, int count);
    
    // Type checking
    bool is_digit(char c);
    bool is_alpha(char c);
    bool is_alnum(char c);
    bool is_whitespace(char c);
    
    // Math utilities
    int abs(int value);
    double abs(double value);
    int min(int a, int b);
    int max(int a, int b);
    double min(double a, double b);
    double max(double a, double b);
    
    // Array utilities
    template<typename T>
    int find(const std::vector<T>& vec, const T& value) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] == value) return i;
        }
        return -1;
    }
    
    template<typename T>
    bool contains(const std::vector<T>& vec, const T& value) {
        return find(vec, value) != -1;
    }
    
    // Random utilities
    int random_int(int min, int max);
    double random_double(double min, double max);
    
    // Hash utilities
    uint32_t hash_string(const std::string& str);
    uint32_t hash_int(int value);
}

} // namespace stdlib
} // namespace sapphire

// C API for code generation
extern "C" {
    const char* sapphire_utils_to_string_int(int value);
    const char* sapphire_utils_to_string_double(double value);
    int sapphire_utils_to_int(const char* str);
    double sapphire_utils_to_double(const char* str);
    
    const char* sapphire_utils_to_upper(const char* str);
    const char* sapphire_utils_to_lower(const char* str);
    const char* sapphire_utils_trim(const char* str);
    
    bool sapphire_utils_starts_with(const char* str, const char* prefix);
    bool sapphire_utils_ends_with(const char* str, const char* suffix);
    bool sapphire_utils_contains(const char* str, const char* substring);
    
    int sapphire_utils_random_int(int min, int max);
    double sapphire_utils_random_double(double min, double max);
    
    uint32_t sapphire_utils_hash_string(const char* str);
}

#endif // SAPPHIRE_STDLIB_UTILS_H
