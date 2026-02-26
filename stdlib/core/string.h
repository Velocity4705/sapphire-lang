#ifndef SAPPHIRE_STDLIB_STRING_H
#define SAPPHIRE_STDLIB_STRING_H

#include <cstddef>
#include <cstring>
#include <vector>
#include <string>

namespace sapphire {
namespace stdlib {

/**
 * String - UTF-8 String Type
 * 
 * Dynamic string with UTF-8 support.
 */
class String {
private:
    char* data_;
    size_t length_;
    size_t capacity_;
    
    void ensure_capacity(size_t new_capacity);
    
public:
    // Constructors
    String();
    String(const char* str);
    String(const char* str, size_t length);
    String(const String& other);
    String(String&& other) noexcept;
    String(const std::string& str);
    
    // Destructor
    ~String();
    
    // Assignment
    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;
    
    // Basic properties
    size_t length() const { return length_; }
    size_t size() const { return length_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return length_ == 0; }
    const char* c_str() const { return data_; }
    const char* data() const { return data_; }
    
    // Access
    char operator[](size_t index) const;
    char& operator[](size_t index);
    char at(size_t index) const;
    
    // Concatenation
    String operator+(const String& other) const;
    String& operator+=(const String& other);
    String& append(const String& other);
    String& append(const char* str);
    String& append(char c);
    
    // Comparison
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;
    bool operator<(const String& other) const;
    bool operator>(const String& other) const;
    bool operator<=(const String& other) const;
    bool operator>=(const String& other) const;
    
    // Substring
    String substring(size_t start) const;
    String substring(size_t start, size_t length) const;
    String substr(size_t start, size_t length) const;
    
    // Search
    bool contains(const String& substr) const;
    int index_of(const String& substr) const;
    int index_of(const String& substr, size_t start) const;
    int last_index_of(const String& substr) const;
    bool starts_with(const String& prefix) const;
    bool ends_with(const String& suffix) const;
    
    // Modification
    String to_upper() const;
    String to_lower() const;
    String trim() const;
    String trim_left() const;
    String trim_right() const;
    String replace(const String& from, const String& to) const;
    
    // Split/Join
    std::vector<String> split(const String& delimiter) const;
    std::vector<String> split(char delimiter) const;
    static String join(const std::vector<String>& strings, const String& delimiter);
    
    // Conversion
    int to_int() const;
    double to_double() const;
    std::string to_std_string() const;
    
    // Static constructors
    static String from_int(int value);
    static String from_double(double value);
    static String from_bool(bool value);
    
    // Clear
    void clear();
    
    // Reserve
    void reserve(size_t capacity);
};

} // namespace stdlib
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Constructors
    void* sapphire_string_new();
    void* sapphire_string_from_cstr(const char* str);
    void sapphire_string_delete(void* str);
    
    // Properties
    size_t sapphire_string_length(void* str);
    const char* sapphire_string_cstr(void* str);
    
    // Operations
    void* sapphire_string_concat(void* str1, void* str2);
    void* sapphire_string_substring(void* str, size_t start, size_t length);
    bool sapphire_string_contains(void* str, void* substr);
    void* sapphire_string_to_upper(void* str);
    void* sapphire_string_to_lower(void* str);
}

#endif // SAPPHIRE_STDLIB_STRING_H
