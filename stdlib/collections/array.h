#pragma once

#include <cstdint>
#include <cstddef>

namespace sapphire {
namespace stdlib {
namespace Collections {

// Dynamic Array Structure
struct DynamicArray {
    void** data;        // Array of void pointers for generic storage
    size_t size;        // Current number of elements
    size_t capacity;    // Current capacity
    size_t element_size; // Size of each element in bytes
    
    DynamicArray(size_t elem_size = sizeof(int64_t));
    ~DynamicArray();
    
    // Core operations
    void push(const void* element);
    void* get(size_t index);
    void set(size_t index, const void* element);
    void* pop();
    void clear();
    
    // Utility
    bool empty() const;
    void reserve(size_t new_capacity);
    void resize();
};

// Factory functions
DynamicArray* create_array(size_t element_size = sizeof(int64_t));
void destroy_array(DynamicArray* array);

// Array operations
void array_push(DynamicArray* array, const void* element);
void* array_get(DynamicArray* array, size_t index);
void array_set(DynamicArray* array, size_t index, const void* element);
void* array_pop(DynamicArray* array);
size_t array_size(DynamicArray* array);
size_t array_capacity(DynamicArray* array);
void array_clear(DynamicArray* array);
bool array_empty(DynamicArray* array);

// Specialized functions for common types
void array_push_int(DynamicArray* array, int64_t value);
int64_t array_get_int(DynamicArray* array, size_t index);
void array_set_int(DynamicArray* array, size_t index, int64_t value);

void array_push_double(DynamicArray* array, double value);
double array_get_double(DynamicArray* array, size_t index);
void array_set_double(DynamicArray* array, size_t index, double value);

void array_push_string(DynamicArray* array, const char* value);
const char* array_get_string(DynamicArray* array, size_t index);
void array_set_string(DynamicArray* array, size_t index, const char* value);

} // namespace Collections
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Array lifecycle
    void* sapphire_array_create();
    void* sapphire_array_create_typed(int64_t element_size);
    void sapphire_array_destroy(void* array);
    
    // Core operations
    void sapphire_array_push_int(void* array, int64_t value);
    int64_t sapphire_array_get_int(void* array, int64_t index);
    void sapphire_array_set_int(void* array, int64_t index, int64_t value);
    int64_t sapphire_array_pop_int(void* array);
    
    void sapphire_array_push_double(void* array, double value);
    double sapphire_array_get_double(void* array, int64_t index);
    void sapphire_array_set_double(void* array, int64_t index, double value);
    double sapphire_array_pop_double(void* array);
    
    void sapphire_array_push_string(void* array, const char* value);
    const char* sapphire_array_get_string(void* array, int64_t index);
    void sapphire_array_set_string(void* array, int64_t index, const char* value);
    const char* sapphire_array_pop_string(void* array);
    
    // Utility functions
    int64_t sapphire_array_size(void* array);
    int64_t sapphire_array_capacity(void* array);
    void sapphire_array_clear(void* array);
    bool sapphire_array_empty(void* array);
}