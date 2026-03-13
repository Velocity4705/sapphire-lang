#include "array.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace sapphire {
namespace stdlib {
namespace Collections {

// DynamicArray Implementation
DynamicArray::DynamicArray(size_t elem_size) 
    : data(nullptr), size(0), capacity(0), element_size(elem_size) {
    reserve(8); // Start with capacity of 8
}

DynamicArray::~DynamicArray() {
    clear();
    if (data) {
        free(data);
        data = nullptr;
    }
}

void DynamicArray::push(const void* element) {
    if (size >= capacity) {
        resize();
    }
    
    // Allocate memory for the new element
    void* new_element = malloc(element_size);
    if (!new_element) {
        throw std::bad_alloc();
    }
    
    // Copy the element data
    memcpy(new_element, element, element_size);
    data[size] = new_element;
    size++;
}

void* DynamicArray::get(size_t index) {
    if (index >= size) {
        return nullptr; // Out of bounds
    }
    return data[index];
}

void DynamicArray::set(size_t index, const void* element) {
    if (index >= size) {
        return; // Out of bounds
    }
    
    // Free existing element
    if (data[index]) {
        free(data[index]);
    }
    
    // Allocate and copy new element
    void* new_element = malloc(element_size);
    if (new_element) {
        memcpy(new_element, element, element_size);
        data[index] = new_element;
    }
}

void* DynamicArray::pop() {
    if (size == 0) {
        return nullptr;
    }
    
    size--;
    void* element = data[size];
    data[size] = nullptr;
    return element;
}

void DynamicArray::clear() {
    for (size_t i = 0; i < size; i++) {
        if (data[i]) {
            free(data[i]);
            data[i] = nullptr;
        }
    }
    size = 0;
}

bool DynamicArray::empty() const {
    return size == 0;
}

void DynamicArray::reserve(size_t new_capacity) {
    if (new_capacity <= capacity) {
        return;
    }
    
    void** new_data = (void**)realloc(data, new_capacity * sizeof(void*));
    if (!new_data) {
        throw std::bad_alloc();
    }
    
    // Initialize new slots to nullptr
    for (size_t i = capacity; i < new_capacity; i++) {
        new_data[i] = nullptr;
    }
    
    data = new_data;
    capacity = new_capacity;
}

void DynamicArray::resize() {
    size_t new_capacity = capacity == 0 ? 8 : capacity * 2;
    reserve(new_capacity);
}

// Factory functions
DynamicArray* create_array(size_t element_size) {
    return new DynamicArray(element_size);
}

void destroy_array(DynamicArray* array) {
    delete array;
}

// Array operations
void array_push(DynamicArray* array, const void* element) {
    if (array) {
        array->push(element);
    }
}

void* array_get(DynamicArray* array, size_t index) {
    if (array) {
        return array->get(index);
    }
    return nullptr;
}

void array_set(DynamicArray* array, size_t index, const void* element) {
    if (array) {
        array->set(index, element);
    }
}

void* array_pop(DynamicArray* array) {
    if (array) {
        return array->pop();
    }
    return nullptr;
}

size_t array_size(DynamicArray* array) {
    return array ? array->size : 0;
}

size_t array_capacity(DynamicArray* array) {
    return array ? array->capacity : 0;
}

void array_clear(DynamicArray* array) {
    if (array) {
        array->clear();
    }
}

bool array_empty(DynamicArray* array) {
    return array ? array->empty() : true;
}

// Specialized functions for integers
void array_push_int(DynamicArray* array, int64_t value) {
    if (array) {
        array->push(&value);
    }
}

int64_t array_get_int(DynamicArray* array, size_t index) {
    if (array) {
        void* element = array->get(index);
        if (element) {
            return *(int64_t*)element;
        }
    }
    return 0;
}

void array_set_int(DynamicArray* array, size_t index, int64_t value) {
    if (array) {
        array->set(index, &value);
    }
}

// Specialized functions for doubles
void array_push_double(DynamicArray* array, double value) {
    if (array) {
        array->push(&value);
    }
}

double array_get_double(DynamicArray* array, size_t index) {
    if (array) {
        void* element = array->get(index);
        if (element) {
            return *(double*)element;
        }
    }
    return 0.0;
}

void array_set_double(DynamicArray* array, size_t index, double value) {
    if (array) {
        array->set(index, &value);
    }
}

// Specialized functions for strings
void array_push_string(DynamicArray* array, const char* value) {
    if (array && value) {
        // Store a copy of the string
        size_t len = strlen(value) + 1;
        char* str_copy = (char*)malloc(len);
        if (str_copy) {
            strcpy(str_copy, value);
            array->push(&str_copy);
        }
    }
}

const char* array_get_string(DynamicArray* array, size_t index) {
    if (array) {
        void* element = array->get(index);
        if (element) {
            return *(const char**)element;
        }
    }
    return nullptr;
}

void array_set_string(DynamicArray* array, size_t index, const char* value) {
    if (array && value) {
        // Free existing string if any
        void* existing = array->get(index);
        if (existing) {
            char* existing_str = *(char**)existing;
            if (existing_str) {
                free(existing_str);
            }
        }
        
        // Store a copy of the new string
        size_t len = strlen(value) + 1;
        char* str_copy = (char*)malloc(len);
        if (str_copy) {
            strcpy(str_copy, value);
            array->set(index, &str_copy);
        }
    }
}

} // namespace Collections
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    void* sapphire_array_create() {
        return sapphire::stdlib::Collections::create_array(sizeof(int64_t));
    }
    
    void* sapphire_array_create_typed(int64_t element_size) {
        return sapphire::stdlib::Collections::create_array((size_t)element_size);
    }
    
    void sapphire_array_destroy(void* array) {
        if (array) {
            sapphire::stdlib::Collections::destroy_array(
                (sapphire::stdlib::Collections::DynamicArray*)array);
        }
    }
    
    // Integer operations
    void sapphire_array_push_int(void* array, int64_t value) {
        if (array) {
            sapphire::stdlib::Collections::array_push_int(
                (sapphire::stdlib::Collections::DynamicArray*)array, value);
        }
    }
    
    int64_t sapphire_array_get_int(void* array, int64_t index) {
        if (array && index >= 0) {
            return sapphire::stdlib::Collections::array_get_int(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index);
        }
        return 0;
    }
    
    void sapphire_array_set_int(void* array, int64_t index, int64_t value) {
        if (array && index >= 0) {
            sapphire::stdlib::Collections::array_set_int(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index, value);
        }
    }
    
    int64_t sapphire_array_pop_int(void* array) {
        if (array) {
            auto* arr = (sapphire::stdlib::Collections::DynamicArray*)array;
            void* element = sapphire::stdlib::Collections::array_pop(arr);
            if (element) {
                int64_t value = *(int64_t*)element;
                free(element);
                return value;
            }
        }
        return 0;
    }
    
    // Double operations
    void sapphire_array_push_double(void* array, double value) {
        if (array) {
            sapphire::stdlib::Collections::array_push_double(
                (sapphire::stdlib::Collections::DynamicArray*)array, value);
        }
    }
    
    double sapphire_array_get_double(void* array, int64_t index) {
        if (array && index >= 0) {
            return sapphire::stdlib::Collections::array_get_double(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index);
        }
        return 0.0;
    }
    
    void sapphire_array_set_double(void* array, int64_t index, double value) {
        if (array && index >= 0) {
            sapphire::stdlib::Collections::array_set_double(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index, value);
        }
    }
    
    double sapphire_array_pop_double(void* array) {
        if (array) {
            auto* arr = (sapphire::stdlib::Collections::DynamicArray*)array;
            void* element = sapphire::stdlib::Collections::array_pop(arr);
            if (element) {
                double value = *(double*)element;
                free(element);
                return value;
            }
        }
        return 0.0;
    }
    
    // String operations
    void sapphire_array_push_string(void* array, const char* value) {
        if (array) {
            sapphire::stdlib::Collections::array_push_string(
                (sapphire::stdlib::Collections::DynamicArray*)array, value);
        }
    }
    
    const char* sapphire_array_get_string(void* array, int64_t index) {
        if (array && index >= 0) {
            return sapphire::stdlib::Collections::array_get_string(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index);
        }
        return nullptr;
    }
    
    void sapphire_array_set_string(void* array, int64_t index, const char* value) {
        if (array && index >= 0) {
            sapphire::stdlib::Collections::array_set_string(
                (sapphire::stdlib::Collections::DynamicArray*)array, (size_t)index, value);
        }
    }
    
    const char* sapphire_array_pop_string(void* array) {
        if (array) {
            auto* arr = (sapphire::stdlib::Collections::DynamicArray*)array;
            void* element = sapphire::stdlib::Collections::array_pop(arr);
            if (element) {
                const char* value = *(const char**)element;
                free(element);
                return value; // Note: caller should free this
            }
        }
        return nullptr;
    }
    
    // Utility functions
    int64_t sapphire_array_size(void* array) {
        if (array) {
            return (int64_t)sapphire::stdlib::Collections::array_size(
                (sapphire::stdlib::Collections::DynamicArray*)array);
        }
        return 0;
    }
    
    int64_t sapphire_array_capacity(void* array) {
        if (array) {
            return (int64_t)sapphire::stdlib::Collections::array_capacity(
                (sapphire::stdlib::Collections::DynamicArray*)array);
        }
        return 0;
    }
    
    void sapphire_array_clear(void* array) {
        if (array) {
            sapphire::stdlib::Collections::array_clear(
                (sapphire::stdlib::Collections::DynamicArray*)array);
        }
    }
    
    bool sapphire_array_empty(void* array) {
        if (array) {
            return sapphire::stdlib::Collections::array_empty(
                (sapphire::stdlib::Collections::DynamicArray*)array);
        }
        return true;
    }
}