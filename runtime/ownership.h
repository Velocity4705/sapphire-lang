#ifndef SAPPHIRE_OWNERSHIP_H
#define SAPPHIRE_OWNERSHIP_H

#include "allocator.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>

namespace sapphire {
namespace runtime {

/**
 * Ownership Errors
 */
class OwnershipError : public std::runtime_error {
public:
    explicit OwnershipError(const std::string& msg) : std::runtime_error(msg) {}
};

class UseAfterMoveError : public OwnershipError {
public:
    explicit UseAfterMoveError(void* ptr);
};

class OwnershipViolationError : public OwnershipError {
public:
    OwnershipViolationError(void* ptr, uint64_t expected, uint64_t actual);
};

/**
 * Ownership Info
 * 
 * Tracks ownership information for an object.
 */
struct OwnershipInfo {
    uint64_t owner_id;      // Thread/context that owns this object
    uint32_t move_count;    // Number of times moved
    bool is_moved;          // Whether object has been moved
    bool is_shared;         // Whether object has shared ownership (RC/Arc)
    
    OwnershipInfo()
        : owner_id(0), move_count(0), is_moved(false), is_shared(false) {}
};

/**
 * Ownership Tracker
 * 
 * Tracks ownership of heap-allocated objects.
 * Provides move semantics and ownership validation.
 */
class OwnershipTracker {
private:
    static std::unordered_map<void*, OwnershipInfo> ownership_map_;
    static std::mutex mutex_;
    
public:
    // Get current thread ID
    static uint64_t current_thread_id();
    
    // Track object ownership
    static void set_owner(void* ptr, uint64_t owner_id);
    static uint64_t get_owner(void* ptr);
    static bool is_owner(void* ptr, uint64_t owner_id);
    static bool has_owner(void* ptr);
    
    // Move semantics
    static void mark_moved(void* ptr);
    static bool is_moved(void* ptr);
    static void transfer_ownership(void* ptr, uint64_t new_owner);
    static uint32_t get_move_count(void* ptr);
    
    // Shared ownership (for RC/Arc)
    static void mark_shared(void* ptr);
    static bool is_shared(void* ptr);
    
    // Untrack object (when freed)
    static void untrack(void* ptr);
    
    // Validation
    static void check_ownership(void* ptr, uint64_t owner_id);
    static void check_not_moved(void* ptr);
    static void check_can_access(void* ptr, uint64_t accessor_id);
    
    // Statistics
    static size_t tracked_count();
    static size_t moved_count();
    static size_t shared_count();
    
    // Clear all tracking (for testing)
    static void clear();
};

/**
 * Move Helper
 * 
 * Marks an object as moved and returns an rvalue reference.
 */
template<typename T>
T&& move(T& obj) {
    void* ptr = &obj;
    OwnershipTracker::mark_moved(ptr);
    return static_cast<T&&>(obj);
}

/**
 * Check if object was moved
 */
template<typename T>
bool is_moved(const T& obj) {
    void* ptr = const_cast<T*>(&obj);
    return OwnershipTracker::is_moved(ptr);
}

/**
 * Owned<T> - Wrapper for owned objects
 * 
 * RAII wrapper that tracks ownership.
 */
template<typename T>
class Owned {
private:
    T* ptr_;
    
public:
    // Construct from raw pointer
    explicit Owned(T* ptr = nullptr) : ptr_(ptr) {
        if (ptr_) {
            OwnershipTracker::set_owner(ptr_, OwnershipTracker::current_thread_id());
        }
    }
    
    // Deleted copy constructor (ownership is exclusive)
    Owned(const Owned&) = delete;
    Owned& operator=(const Owned&) = delete;
    
    // Move constructor
    Owned(Owned&& other) noexcept : ptr_(other.ptr_) {
        if (ptr_) {
            OwnershipTracker::transfer_ownership(ptr_, OwnershipTracker::current_thread_id());
        }
        other.ptr_ = nullptr;
    }
    
    // Move assignment
    Owned& operator=(Owned&& other) noexcept {
        if (this != &other) {
            if (ptr_) {
                delete ptr_;
                OwnershipTracker::untrack(ptr_);
            }
            ptr_ = other.ptr_;
            if (ptr_) {
                OwnershipTracker::transfer_ownership(ptr_, OwnershipTracker::current_thread_id());
            }
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    // Destructor
    ~Owned() {
        if (ptr_) {
            delete ptr_;
            OwnershipTracker::untrack(ptr_);
        }
    }
    
    // Get raw pointer
    T* get() const {
        if (ptr_) {
            OwnershipTracker::check_not_moved(ptr_);
        }
        return ptr_;
    }
    
    // Dereference
    T& operator*() const {
        return *get();
    }
    
    T* operator->() const {
        return get();
    }
    
    // Check if valid
    explicit operator bool() const {
        return ptr_ != nullptr && !OwnershipTracker::is_moved(ptr_);
    }
    
    // Release ownership (returns raw pointer)
    T* release() {
        T* result = ptr_;
        ptr_ = nullptr;
        if (result) {
            OwnershipTracker::untrack(result);
        }
        return result;
    }
    
    // Reset to null
    void reset(T* ptr = nullptr) {
        if (ptr_) {
            delete ptr_;
            OwnershipTracker::untrack(ptr_);
        }
        ptr_ = ptr;
        if (ptr_) {
            OwnershipTracker::set_owner(ptr_, OwnershipTracker::current_thread_id());
        }
    }
};

} // namespace runtime
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Track ownership
    void sapphire_set_owner(void* ptr, uint64_t owner_id);
    uint64_t sapphire_get_owner(void* ptr);
    bool sapphire_is_owner(void* ptr, uint64_t owner_id);
    
    // Move semantics
    void sapphire_mark_moved(void* ptr);
    bool sapphire_is_moved(void* ptr);
    void sapphire_transfer_ownership(void* ptr, uint64_t new_owner);
    
    // Shared ownership
    void sapphire_mark_shared(void* ptr);
    bool sapphire_is_shared(void* ptr);
    
    // Validation
    void sapphire_check_ownership(void* ptr, uint64_t owner_id);
    void sapphire_check_not_moved(void* ptr);
    
    // Get current thread ID
    uint64_t sapphire_current_thread_id();
}

#endif // SAPPHIRE_OWNERSHIP_H
