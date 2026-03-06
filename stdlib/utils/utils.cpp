#include "utils.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <random>
#include <sstream>

namespace sapphire {
namespace stdlib {

namespace Utils {
    // String conversion
    std::string to_string(int value) {
        return std::to_string(value);
    }
    
    std::string to_string(double value) {
        return std::to_string(value);
    }
    
    std::string to_string(bool value) {
        return value ? "true" : "false";
    }
    
    int to_int(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (...) {
            return 0;
        }
    }
    
    double to_double(const std::string& str) {
        try {
            return std::stod(str);
        } catch (...) {
            return 0.0;
        }
    }
    
    bool to_bool(const std::string& str) {
        std::string lower = to_lower(str);
        return lower == "true" || lower == "1" || lower == "yes";
    }
    
    // Case conversion
    std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Trimming
    std::string ltrim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        return (start == std::string::npos) ? "" : str.substr(start);
    }
    
    std::string rtrim(const std::string& str) {
        size_t end = str.find_last_not_of(" \t\n\r");
        return (end == std::string::npos) ? "" : str.substr(0, end + 1);
    }
    
    std::string trim(const std::string& str) {
        return ltrim(rtrim(str));
    }
    
    // String splitting and joining
    std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = str.find(delimiter);
        
        while (end != std::string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        result.push_back(str.substr(start));
        return result;
    }
    
    std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        if (strings.empty()) return "";
        
        std::string result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
        return result;
    }
    
    // String predicates
    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.find(prefix) == 0;
    }
    
    bool ends_with(const std::string& str, const std::string& suffix) {
        if (suffix.length() > str.length()) return false;
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    
    bool contains(const std::string& str, const std::string& substring) {
        return str.find(substring) != std::string::npos;
    }
    
    // String replacement
    std::string replace(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
    
    std::string repeat(const std::string& str, int count) {
        std::string result;
        for (int i = 0; i < count; ++i) {
            result += str;
        }
        return result;
    }
    
    // Character classification
    bool is_digit(char c) {
        return std::isdigit(static_cast<unsigned char>(c)) != 0;
    }
    
    bool is_alpha(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) != 0;
    }
    
    bool is_alnum(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) != 0;
    }
    
    bool is_whitespace(char c) {
        return std::isspace(static_cast<unsigned char>(c)) != 0;
    }
    
    // Math utilities
    int abs(int value) {
        return value < 0 ? -value : value;
    }
    
    double abs(double value) {
        return value < 0.0 ? -value : value;
    }
    
    int min(int a, int b) {
        return a < b ? a : b;
    }
    
    int max(int a, int b) {
        return a > b ? a : b;
    }
    
    double min(double a, double b) {
        return a < b ? a : b;
    }
    
    double max(double a, double b) {
        return a > b ? a : b;
    }
    
    // Random utilities
    int random_int(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    
    double random_double(double min, double max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }
    
    // Hash utilities
    uint32_t hash_string(const std::string& str) {
        uint32_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return hash;
    }
    
    uint32_t hash_int(int value) {
        uint32_t hash = value;
        hash = ((hash << 5) + hash) ^ (hash >> 17);
        return hash;
    }
}

} // namespace stdlib
} // namespace sapphire

// C API implementations
extern "C" {
    static thread_local std::string g_string_buffer;
    
    const char* sapphire_utils_to_string_int(int value) {
        g_string_buffer = sapphire::stdlib::Utils::to_string(value);
        return g_string_buffer.c_str();
    }
    
    const char* sapphire_utils_to_string_double(double value) {
        g_string_buffer = sapphire::stdlib::Utils::to_string(value);
        return g_string_buffer.c_str();
    }
    
    int sapphire_utils_to_int(const char* str) {
        return sapphire::stdlib::Utils::to_int(str);
    }
    
    double sapphire_utils_to_double(const char* str) {
        return sapphire::stdlib::Utils::to_double(str);
    }
    
    const char* sapphire_utils_to_upper(const char* str) {
        g_string_buffer = sapphire::stdlib::Utils::to_upper(str);
        return g_string_buffer.c_str();
    }
    
    const char* sapphire_utils_to_lower(const char* str) {
        g_string_buffer = sapphire::stdlib::Utils::to_lower(str);
        return g_string_buffer.c_str();
    }
    
    const char* sapphire_utils_trim(const char* str) {
        g_string_buffer = sapphire::stdlib::Utils::trim(str);
        return g_string_buffer.c_str();
    }
    
    bool sapphire_utils_starts_with(const char* str, const char* prefix) {
        return sapphire::stdlib::Utils::starts_with(str, prefix);
    }
    
    bool sapphire_utils_ends_with(const char* str, const char* suffix) {
        return sapphire::stdlib::Utils::ends_with(str, suffix);
    }
    
    bool sapphire_utils_contains(const char* str, const char* substring) {
        return sapphire::stdlib::Utils::contains(str, substring);
    }
    
    int sapphire_utils_random_int(int min, int max) {
        return sapphire::stdlib::Utils::random_int(min, max);
    }
    
    double sapphire_utils_random_double(double min, double max) {
        return sapphire::stdlib::Utils::random_double(min, max);
    }
    
    uint32_t sapphire_utils_hash_string(const char* str) {
        return sapphire::stdlib::Utils::hash_string(str);
    }
}
