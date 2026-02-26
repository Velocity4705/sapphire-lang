#include "allocator.h"
#include <cstdlib>
#include <cstring>
#include <new>
#include <iostream>

namespace sapphire {
namespace runtime {

// Global allocator instance
Allocator* global_allocator = nullptr;

//=============================================================================
// SizeClass Implementation
//=============================================================================

SizeClass::SizeClass(size_t size)
    : size_(size), free_list_(nullptr), allocated_count_(0), freed_count_(0) {
}

void* SizeClass::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Try to allocate from free list
    if (free_list_) {
        FreeNode* node = free_list_;
        free_list_ = node->next;
        allocated_count_++;
        return node;
    }
    
    // Allocate new block
    void* ptr = std::malloc(size_);
    if (!ptr) {
        throw std::bad_alloc();
    }
    allocated_count_++;
    return ptr;
}

void SizeClass::free(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Add to free list
    FreeNode* node = static_cast<FreeNode*>(ptr);
    node->next = free_list_;
    node->size = size_;
    free_list_ = node;
    freed_count_++;
}

//=============================================================================
// BumpAllocator Implementation
//=============================================================================

BumpAllocator::BumpAllocator()
    : start_(nullptr), current_(nullptr), end_(nullptr), total_allocated_(0) {
    allocate_chunk();
}

BumpAllocator::~BumpAllocator() {
    if (start_) {
        std::free(start_);
    }
}

void BumpAllocator::allocate_chunk() {
    start_ = static_cast<char*>(std::malloc(CHUNK_SIZE));
    if (!start_) {
        throw std::bad_alloc();
    }
    current_ = start_;
    end_ = start_ + CHUNK_SIZE;
}

void* BumpAllocator::allocate(size_t size) {
    // Align to 8 bytes
    size = (size + 7) & ~7;
    
    if (current_ + size > end_) {
        // Not enough space, need to allocate from global allocator
        return nullptr;
    }
    
    void* ptr = current_;
    current_ += size;
    total_allocated_ += size;
    return ptr;
}

void BumpAllocator::reset() {
    current_ = start_;
    total_allocated_ = 0;
}

//=============================================================================
// Allocator Implementation
//=============================================================================

constexpr size_t Allocator::SIZE_CLASSES[];

Allocator::Allocator()
    : total_allocated_(0), total_freed_(0), allocation_count_(0) {
    
    // Initialize size classes
    for (size_t i = 0; i < NUM_SIZE_CLASSES; i++) {
        size_classes_[i] = new SizeClass(SIZE_CLASSES[i]);
    }
}

Allocator::~Allocator() {
    // Clean up size classes
    for (size_t i = 0; i < NUM_SIZE_CLASSES; i++) {
        delete size_classes_[i];
    }
}

int Allocator::get_size_class_index(size_t size) const {
    // Find appropriate size class
    for (size_t i = 0; i < NUM_SIZE_CLASSES; i++) {
        if (size <= SIZE_CLASSES[i]) {
            return i;
        }
    }
    return -1;  // Too large for size classes
}

void* Allocator::allocate_small(size_t size, uint64_t type_id) {
    // Get size class
    int index = get_size_class_index(size + sizeof(ObjectHeader));
    if (index < 0) {
        return allocate_large(size, type_id);
    }
    
    // Allocate from size class
    void* ptr = size_classes_[index]->allocate();
    if (!ptr) {
        throw std::bad_alloc();
    }
    
    // Initialize header
    ObjectHeader* header = static_cast<ObjectHeader*>(ptr);
    header->type_id = type_id;
    header->size = SIZE_CLASSES[index];
    header->gc_flags = 0;
    
    // Update statistics
    total_allocated_.fetch_add(SIZE_CLASSES[index]);
    allocation_count_.fetch_add(1);
    
    return header->data();
}

void* Allocator::allocate_large(size_t size, uint64_t type_id) {
    std::lock_guard<std::mutex> lock(large_object_mutex_);
    
    // Allocate large block
    size_t total_size = size + sizeof(ObjectHeader);
    void* ptr = std::malloc(total_size);
    if (!ptr) {
        throw std::bad_alloc();
    }
    
    // Initialize header
    ObjectHeader* header = static_cast<ObjectHeader*>(ptr);
    header->type_id = type_id;
    header->size = total_size;
    header->gc_flags = ObjectHeader::LARGE_BIT;
    
    // Update statistics
    total_allocated_.fetch_add(total_size);
    allocation_count_.fetch_add(1);
    
    return header->data();
}

void* Allocator::allocate(size_t size, uint64_t type_id) {
    if (size == 0) {
        size = 1;  // Minimum allocation size
    }
    
    // Choose allocation strategy based on size
    if (size + sizeof(ObjectHeader) <= SMALL_OBJECT_THRESHOLD) {
        return allocate_small(size, type_id);
    } else {
        return allocate_large(size, type_id);
    }
}

void Allocator::free(void* ptr) {
    if (!ptr) return;
    
    // Get header
    ObjectHeader* header = get_header(ptr);
    
    // Update statistics
    total_freed_.fetch_add(header->size);
    
    // Free based on size
    if (header->is_large()) {
        std::lock_guard<std::mutex> lock(large_object_mutex_);
        std::free(header);
    } else {
        // Find size class
        int index = get_size_class_index(header->size);
        if (index >= 0) {
            size_classes_[index]->free(header);
        } else {
            // Shouldn't happen, but free anyway
            std::free(header);
        }
    }
}

//=============================================================================
// Initialization
//=============================================================================

void init_allocator() {
    if (!global_allocator) {
        global_allocator = new Allocator();
    }
}

void shutdown_allocator() {
    if (global_allocator) {
        delete global_allocator;
        global_allocator = nullptr;
    }
}

} // namespace runtime
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

extern "C" {

void* sapphire_allocate(size_t size, uint64_t type_id) {
    if (!sapphire::runtime::global_allocator) {
        sapphire::runtime::init_allocator();
    }
    return sapphire::runtime::global_allocator->allocate(size, type_id);
}

void sapphire_free(void* ptr) {
    if (sapphire::runtime::global_allocator) {
        sapphire::runtime::global_allocator->free(ptr);
    }
}

size_t sapphire_memory_usage() {
    if (sapphire::runtime::global_allocator) {
        return sapphire::runtime::global_allocator->current_usage();
    }
    return 0;
}

} // extern "C"
