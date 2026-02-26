#ifndef SAPPHIRE_SAFETY_H
#define SAPPHIRE_SAFETY_H

#include "allocator.h"
#include <stdexcept>
#include <string>
#include <cstdint>

namespace sapphire {
namespace runtime {

/**
 * Safety Errors
 * 
 * Base class for all memory safety errors.
 */
class SafetyError : public std::runtime_error {
public:
    explicit SafetyError(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * Use-After-Free Error
 * 
 * Thrown when accessing memory that has been freed.
 */
class UseAfterFreeError : public SafetyError {
public:
    explicit UseAfterFreeError(void* ptr);
};

/**
 * Double-Free Error
 * 
 * Thrown when freeing memory that has already been freed.
 */
class DoubleFreeError : public SafetyError {
public:
    explicit DoubleFreeError(void* ptr);
};

/**
 * Buffer Overflow Error
 * 
 * Thrown when accessing array out of bounds.
 */
class BufferOverflowError : public SafetyError {
public:
    BufferOverflowError(size_t index, size_t length);
};

/**
 * Type Mismatch Error
 * 
 * Thrown when casting to wrong type.
 */
class TypeMismatchError : public SafetyError {
public:
    TypeMismatchError(uint64_t actual, uint64_t expected);
};

/**
 * Null Pointer Error
 * 
 * Thrown when dereferencing null pointer.
 */
class NullPointerError : public SafetyError {
public:
    NullPointerError();
};

/**
 * Safety Checker
 * 
 * Provides memory safety checks for Sapphire runtime.
 */
class SafetyChecker {
public:
    // Check if pointer is valid (not null, not freed)
    static bool is_valid_pointer(void* ptr);
    
    // Check if pointer was freed
    static bool is_freed(void* ptr);
    
    // Check if pointer is null
    static bool is_null(void* ptr);
    
    // Check array bounds
    static void check_bounds(size_t index, size_t length);
    
    // Check type
    static void check_type(void* ptr, uint64_t expected_type);
    
    // Check null pointer
    static void* check_null(void* ptr);
    
    // Check use-after-free
    static void* check_use_after_free(void* ptr);
    
    // Mark pointer as freed (for use-after-free detection)
    static void mark_freed(void* ptr);
    
    // Poison freed memory
    static void poison_memory(void* ptr, size_t size);
    
    // Check if memory is poisoned
    static bool is_poisoned(void* ptr, size_t size);
};

/**
 * Safe Pointer Wrapper
 * 
 * RAII wrapper for safe pointer access.
 */
template<typename T>
class SafePtr {
public:
    explicit SafePtr(T* ptr) : ptr_(ptr) {
        SafetyChecker::check_null(ptr);
        SafetyChecker::check_use_after_free(ptr);
    }
    
    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    
    operator T*() const { return ptr_; }
    
private:
    T* ptr_;
};

} // namespace runtime
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Check if pointer is valid
    bool sapphire_is_valid_pointer(void* ptr);
    
    // Check if pointer was freed
    bool sapphire_is_freed(void* ptr);
    
    // Check array bounds
    void sapphire_check_bounds(size_t index, size_t length);
    
    // Check type
    void sapphire_check_type(void* ptr, uint64_t expected_type);
    
    // Check null
    void* sapphire_check_null(void* ptr);
    
    // Check use-after-free
    void* sapphire_check_use_after_free(void* ptr);
    
    // Mark as freed
    void sapphire_mark_freed(void* ptr);
}

#endif // SAPPHIRE_SAFETY_H
