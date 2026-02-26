#include "string.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace sapphire {
namespace stdlib {

//=============================================================================
// Private Methods
//=============================================================================

void String::ensure_capacity(size_t new_capacity) {
    // Always allocate at least 1 byte for null terminator
    if (new_capacity < 1) new_capacity = 1;
    
    if (new_capacity <= capacity_) return;
    
    size_t new_cap = capacity_ == 0 ? 16 : capacity_;
    while (new_cap < new_capacity) {
        new_cap *= 2;
    }
    
    char* new_data = new char[new_cap + 1];
    if (data_) {
        std::memcpy(new_data, data_, length_ + 1);
        delete[] data_;
    } else {
        new_data[0] = '\0';
    }
    data_ = new_data;
    capacity_ = new_cap;
}

//=============================================================================
// Constructors
//=============================================================================

String::String() : data_(nullptr), length_(0), capacity_(1) {
    data_ = new char[1];
    data_[0] = '\0';
}

String::String(const char* str) : data_(nullptr), length_(0), capacity_(0) {
    if (str) {
        length_ = std::strlen(str);
        ensure_capacity(length_);
        std::strcpy(data_, str);
    } else {
        data_ = new char[1];
        data_[0] = '\0';
    }
}

String::String(const char* str, size_t length) : data_(nullptr), length_(length), capacity_(0) {
    ensure_capacity(length);
    std::memcpy(data_, str, length);
    data_[length] = '\0';
}

String::String(const String& other) : data_(nullptr), length_(other.length_), capacity_(0) {
    ensure_capacity(length_);
    std::strcpy(data_, other.data_);
}

String::String(String&& other) noexcept 
    : data_(other.data_), length_(other.length_), capacity_(other.capacity_) {
    other.data_ = new char[1];  // Don't leave nullptr
    other.data_[0] = '\0';
    other.length_ = 0;
    other.capacity_ = 1;
}

String::String(const std::string& str) : String(str.c_str()) {}

String::~String() {
    delete[] data_;
}

//=============================================================================
// Assignment
//=============================================================================

String& String::operator=(const String& other) {
    if (this != &other) {
        delete[] data_;
        length_ = other.length_;
        capacity_ = 0;
        data_ = nullptr;
        ensure_capacity(length_);
        std::strcpy(data_, other.data_);
    }
    return *this;
}

String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        delete[] data_;
        data_ = other.data_;
        length_ = other.length_;
        capacity_ = other.capacity_;
        other.data_ = new char[1];  // Don't leave nullptr
        other.data_[0] = '\0';
        other.length_ = 0;
        other.capacity_ = 1;
    }
    return *this;
}

//=============================================================================
// Access
//=============================================================================

char String::operator[](size_t index) const {
    return data_[index];
}

char& String::operator[](size_t index) {
    return data_[index];
}

char String::at(size_t index) const {
    if (index >= length_) {
        throw std::out_of_range("String index out of range");
    }
    return data_[index];
}

//=============================================================================
// Concatenation
//=============================================================================

String String::operator+(const String& other) const {
    String result;
    result.ensure_capacity(length_ + other.length_);
    std::strcpy(result.data_, data_);
    std::strcat(result.data_, other.data_);
    result.length_ = length_ + other.length_;
    return result;
}

String& String::operator+=(const String& other) {
    return append(other);
}

String& String::append(const String& other) {
    ensure_capacity(length_ + other.length_);
    std::strcat(data_, other.data_);
    length_ += other.length_;
    return *this;
}

String& String::append(const char* str) {
    size_t len = std::strlen(str);
    ensure_capacity(length_ + len);
    std::strcat(data_, str);
    length_ += len;
    return *this;
}

String& String::append(char c) {
    ensure_capacity(length_ + 1);
    data_[length_] = c;
    data_[length_ + 1] = '\0';
    length_++;
    return *this;
}

//=============================================================================
// Comparison
//=============================================================================

bool String::operator==(const String& other) const {
    return std::strcmp(data_, other.data_) == 0;
}

bool String::operator!=(const String& other) const {
    return !(*this == other);
}

bool String::operator<(const String& other) const {
    return std::strcmp(data_, other.data_) < 0;
}

bool String::operator>(const String& other) const {
    return std::strcmp(data_, other.data_) > 0;
}

bool String::operator<=(const String& other) const {
    return !(*this > other);
}

bool String::operator>=(const String& other) const {
    return !(*this < other);
}

//=============================================================================
// Substring
//=============================================================================

String String::substring(size_t start) const {
    if (start >= length_) return String();
    return String(data_ + start);
}

String String::substring(size_t start, size_t length) const {
    if (start >= length_) return String();
    size_t actual_length = std::min(length, length_ - start);
    return String(data_ + start, actual_length);
}

String String::substr(size_t start, size_t length) const {
    return substring(start, length);
}

//=============================================================================
// Search
//=============================================================================

bool String::contains(const String& substr) const {
    return std::strstr(data_, substr.data_) != nullptr;
}

int String::index_of(const String& substr) const {
    return index_of(substr, 0);
}

int String::index_of(const String& substr, size_t start) const {
    if (start >= length_) return -1;
    const char* pos = std::strstr(data_ + start, substr.data_);
    return pos ? static_cast<int>(pos - data_) : -1;
}

int String::last_index_of(const String& substr) const {
    int last = -1;
    int pos = 0;
    while ((pos = index_of(substr, pos)) != -1) {
        last = pos;
        pos++;
    }
    return last;
}

bool String::starts_with(const String& prefix) const {
    if (prefix.length_ > length_) return false;
    return std::strncmp(data_, prefix.data_, prefix.length_) == 0;
}

bool String::ends_with(const String& suffix) const {
    if (suffix.length_ > length_) return false;
    return std::strcmp(data_ + length_ - suffix.length_, suffix.data_) == 0;
}

//=============================================================================
// Modification
//=============================================================================

String String::to_upper() const {
    String result(*this);
    for (size_t i = 0; i < length_; i++) {
        result.data_[i] = std::toupper(result.data_[i]);
    }
    return result;
}

String String::to_lower() const {
    String result(*this);
    for (size_t i = 0; i < length_; i++) {
        result.data_[i] = std::tolower(result.data_[i]);
    }
    return result;
}

String String::trim() const {
    return trim_left().trim_right();
}

String String::trim_left() const {
    size_t start = 0;
    while (start < length_ && std::isspace(data_[start])) {
        start++;
    }
    return substring(start);
}

String String::trim_right() const {
    size_t end = length_;
    while (end > 0 && std::isspace(data_[end - 1])) {
        end--;
    }
    return String(data_, end);
}

String String::replace(const String& from, const String& to) const {
    String result;
    size_t pos = 0;
    int found;
    
    while ((found = index_of(from, pos)) != -1) {
        result.append(substring(pos, found - pos));
        result.append(to);
        pos = found + from.length_;
    }
    result.append(substring(pos));
    return result;
}

//=============================================================================
// Split/Join
//=============================================================================

std::vector<String> String::split(const String& delimiter) const {
    std::vector<String> result;
    if (delimiter.empty()) {
        result.push_back(*this);
        return result;
    }
    
    size_t pos = 0;
    int found;
    
    while ((found = index_of(delimiter, pos)) != -1) {
        result.push_back(substring(pos, found - pos));
        pos = found + delimiter.length_;
    }
    result.push_back(substring(pos));
    return result;
}

std::vector<String> String::split(char delimiter) const {
    char delim_str[2] = {delimiter, '\0'};
    return split(String(delim_str));
}

String String::join(const std::vector<String>& strings, const String& delimiter) {
    if (strings.empty()) return String();
    
    String result = strings[0];
    for (size_t i = 1; i < strings.size(); i++) {
        result.append(delimiter);
        result.append(strings[i]);
    }
    return result;
}

//=============================================================================
// Conversion
//=============================================================================

int String::to_int() const {
    return std::atoi(data_);
}

double String::to_double() const {
    return std::atof(data_);
}

std::string String::to_std_string() const {
    return std::string(data_);
}

String String::from_int(int value) {
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%d", value);
    return String(buffer);
}

String String::from_double(double value) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%g", value);
    return String(buffer);
}

String String::from_bool(bool value) {
    return String(value ? "true" : "false");
}

//=============================================================================
// Clear/Reserve
//=============================================================================

void String::clear() {
    length_ = 0;
    if (data_) {
        data_[0] = '\0';
    }
}

void String::reserve(size_t capacity) {
    ensure_capacity(capacity);
}

} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

using namespace sapphire::stdlib;

extern "C" {

void* sapphire_string_new() {
    return new String();
}

void* sapphire_string_from_cstr(const char* str) {
    return new String(str);
}

void sapphire_string_delete(void* str) {
    delete static_cast<String*>(str);
}

size_t sapphire_string_length(void* str) {
    return static_cast<String*>(str)->length();
}

const char* sapphire_string_cstr(void* str) {
    return static_cast<String*>(str)->c_str();
}

void* sapphire_string_concat(void* str1, void* str2) {
    String* s1 = static_cast<String*>(str1);
    String* s2 = static_cast<String*>(str2);
    return new String(*s1 + *s2);
}

void* sapphire_string_substring(void* str, size_t start, size_t length) {
    return new String(static_cast<String*>(str)->substring(start, length));
}

bool sapphire_string_contains(void* str, void* substr) {
    String* s = static_cast<String*>(str);
    String* sub = static_cast<String*>(substr);
    return s->contains(*sub);
}

void* sapphire_string_to_upper(void* str) {
    return new String(static_cast<String*>(str)->to_upper());
}

void* sapphire_string_to_lower(void* str) {
    return new String(static_cast<String*>(str)->to_lower());
}

} // extern "C"
