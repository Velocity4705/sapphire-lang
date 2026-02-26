#ifndef SAPPHIRE_ALLOCATOR_H
#define SAPPHIRE_ALLOCATOR_H

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <mutex>

namespace sapphire {
namespace runtime {

/**
 * Object Header
 * 
 * Every heap-allocated object has a header with metadata for the GC.
 * Header is 16 bytes, aligned to 8 bytes.
 */
struct ObjectHeader {
    uint64_t type_id;      // Type information (for RTTI and GC)
    uint32_t size;         // Object size (including header)
    uint32_t gc_flags;     // GC flags (mark bit, etc.)
    
    // GC flag bits
    static constexpr uint32_t MARK_BIT = 0x01;
    static constexpr uint32_t LARGE_BIT = 0x02;
    static constexpr uint32_t PINNED_BIT = 0x04;
    static constexpr uint32_t FREED_BIT = 0x08;
    static constexpr uint32_t POISONED_BIT = 0x10;
    
    // Flag accessors
    bool is_marked() const { return gc_flags & MARK_BIT; }
    void set_marked(bool marked) {
        if (marked) {
            gc_flags |= MARK_BIT;
        } else {
            gc_flags &= ~MARK_BIT;
        }
    }
    
    bool is_large() const { return gc_flags & LARGE_BIT; }
    void set_large(bool large) {
        if (large) {
            gc_flags |= LARGE_BIT;
        } else {
            gc_flags &= ~LARGE_BIT;
        }
    }
    
    bool is_pinned() const { return gc_flags & PINNED_BIT; }
    void set_pinned(bool pinned) {
        if (pinned) {
            gc_flags |= PINNED_BIT;
        } else {
            gc_flags &= ~PINNED_BIT;
        }
    }
    
    bool is_freed() const { return gc_flags & FREED_BIT; }
    void set_freed(bool freed) {
        if (freed) {
            gc_flags |= FREED_BIT;
        } else {
            gc_flags &= ~FREED_BIT;
        }
    }
    
    bool is_poisoned() const { return gc_flags & POISONED_BIT; }
    void set_poisoned(bool poisoned) {
        if (poisoned) {
            gc_flags |= POISONED_BIT;
        } else {
            gc_flags &= ~POISONED_BIT;
        }
    }
    
    // Get pointer to object data (skip header)
    void* data() { return reinterpret_cast<char*>(this) + sizeof(ObjectHeader); }
    const void* data() const { return reinterpret_cast<const char*>(this) + sizeof(ObjectHeader); }
    
    // Get header from data pointer
    static ObjectHeader* from_data(void* data) {
        return reinterpret_cast<ObjectHeader*>(
            reinterpret_cast<char*>(data) - sizeof(ObjectHeader));
    }
};

static_assert(sizeof(ObjectHeader) == 16, "ObjectHeader must be 16 bytes");

/**
 * Free List Node
 * 
 * Used to track free blocks in the allocator.
 */
struct FreeNode {
    FreeNode* next;
    size_t size;
};

/**
 * Size Class
 * 
 * Each size class has its own free list for fast allocation.
 */
class SizeClass {
public:
    explicit SizeClass(size_t size);
    
    // Allocate from this size class
    void* allocate();
    
    // Free to this size class
    void free(void* ptr);
    
    // Get size for this class
    size_t size() const { return size_; }
    
    // Statistics
    size_t allocated_count() const { return allocated_count_; }
    size_t freed_count() const { return freed_count_; }
    
private:
    size_t size_;
    FreeNode* free_list_;
    std::mutex mutex_;
    size_t allocated_count_;
    size_t freed_count_;
};

/**
 * Bump Allocator
 * 
 * Fast thread-local allocator using bump pointer allocation.
 */
class BumpAllocator {
public:
    BumpAllocator();
    ~BumpAllocator();
    
    // Allocate from bump pointer
    void* allocate(size_t size);
    
    // Check if we have space
    bool has_space(size_t size) const {
        return current_ + size <= end_;
    }
    
    // Reset allocator (for testing)
    void reset();
    
    // Statistics
    size_t total_allocated() const { return total_allocated_; }
    
private:
    static constexpr size_t CHUNK_SIZE = 64 * 1024;  // 64KB chunks
    
    char* start_;
    char* current_;
    char* end_;
    size_t total_allocated_;
    
    void allocate_chunk();
};

/**
 * Global Allocator
 * 
 * Main allocator for the Sapphire runtime.
 * Uses size classes for small objects and direct allocation for large objects.
 */
class Allocator {
public:
    Allocator();
    ~Allocator();
    
    // Allocate object of given size and type
    void* allocate(size_t size, uint64_t type_id);
    
    // Free object (for manual management, not normally used)
    void free(void* ptr);
    
    // Get object header from data pointer
    static ObjectHeader* get_header(void* ptr) {
        return ObjectHeader::from_data(ptr);
    }
    
    // Statistics
    size_t total_allocated() const { return total_allocated_.load(); }
    size_t total_freed() const { return total_freed_.load(); }
    size_t current_usage() const { return total_allocated() - total_freed(); }
    size_t allocation_count() const { return allocation_count_.load(); }
    
    // Size class thresholds
    static constexpr size_t SMALL_OBJECT_THRESHOLD = 256;
    static constexpr size_t SIZE_CLASSES[] = {8, 16, 32, 64, 128, 256};
    static constexpr size_t NUM_SIZE_CLASSES = 6;
    
private:
    SizeClass* size_classes_[NUM_SIZE_CLASSES];
    std::mutex large_object_mutex_;
    
    std::atomic<size_t> total_allocated_;
    std::atomic<size_t> total_freed_;
    std::atomic<size_t> allocation_count_;
    
    // Get size class index for given size
    int get_size_class_index(size_t size) const;
    
    // Allocate small object (< 256 bytes)
    void* allocate_small(size_t size, uint64_t type_id);
    
    // Allocate large object (>= 256 bytes)
    void* allocate_large(size_t size, uint64_t type_id);
};

// Global allocator instance
extern Allocator* global_allocator;

// Initialize allocator
void init_allocator();

// Shutdown allocator
void shutdown_allocator();

} // namespace runtime
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Allocate object
    void* sapphire_allocate(size_t size, uint64_t type_id);
    
    // Free object
    void sapphire_free(void* ptr);
    
    // Get allocation statistics
    size_t sapphire_memory_usage();
}

#endif // SAPPHIRE_ALLOCATOR_H
