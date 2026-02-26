#include "gc.h"
#include <chrono>
#include <cstring>
#include <iostream>
#include <algorithm>

#ifdef __linux__
#include <pthread.h>
#endif

namespace sapphire {
namespace runtime {

// Global GC instance
GarbageCollector* global_gc = nullptr;

//=============================================================================
// Helper Functions
//=============================================================================

static uint64_t get_time_us() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

static void* get_stack_pointer() {
    void* sp;
#ifdef __x86_64__
    asm("mov %%rsp, %0" : "=r"(sp));
#elif defined(__aarch64__)
    asm("mov %0, sp" : "=r"(sp));
#else
    // Fallback: use address of local variable
    sp = &sp;
#endif
    return sp;
}

//=============================================================================
// GarbageCollector Implementation
//=============================================================================

GarbageCollector::GarbageCollector(Allocator* allocator)
    : allocator_(allocator)
    , phase_(GCPhase::IDLE)
    , stack_bottom_(nullptr)
    , stack_top_(nullptr)
    , heap_start_(nullptr)
    , heap_end_(nullptr)
    , gc_threshold_(1024 * 1024)  // 1MB default
    , bytes_at_last_gc_(0)
    , gc_start_time_(0)
{
    // Initialize statistics
    std::memset(&stats_, 0, sizeof(stats_));
    
    // Get stack bounds
    stack_top_ = get_stack_pointer();
    
#ifdef __linux__
    // Get stack bottom from pthread
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    
    void* stack_addr;
    size_t stack_size;
    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    stack_bottom_ = static_cast<char*>(stack_addr) + stack_size;
    
    pthread_attr_destroy(&attr);
#else
    // Fallback: assume 8MB stack
    stack_bottom_ = static_cast<char*>(stack_top_) + (8 * 1024 * 1024);
#endif
    
    // Heap bounds will be updated by allocator
    // For now, use conservative bounds
    heap_start_ = reinterpret_cast<void*>(0x1000);
    heap_end_ = reinterpret_cast<void*>(0x7FFFFFFFFFFF);
}

GarbageCollector::~GarbageCollector() {
}

bool GarbageCollector::should_collect() const {
    size_t current_usage = allocator_->current_usage();
    return current_usage > gc_threshold_;
}

void GarbageCollector::collect() {
    gc_start_time_ = get_time_us();
    
    // Mark phase
    phase_ = GCPhase::MARKING;
    mark();
    
    // Sweep phase
    phase_ = GCPhase::SWEEPING;
    sweep();
    
    // Update statistics
    phase_ = GCPhase::IDLE;
    stats_.collections++;
    stats_.last_pause_us = get_time_us() - gc_start_time_;
    stats_.total_pause_us += stats_.last_pause_us;
    
    // Update threshold (adaptive)
    bytes_at_last_gc_ = allocator_->current_usage();
    gc_threshold_ = bytes_at_last_gc_ * 2;  // Collect when doubled
    if (gc_threshold_ < 1024 * 1024) {
        gc_threshold_ = 1024 * 1024;  // Minimum 1MB
    }
}

//=============================================================================
// Mark Phase
//=============================================================================

void GarbageCollector::mark() {
    stats_.objects_marked = 0;
    
    // Clear work queue
    work_queue_.clear();
    
    // Scan roots
    mark_roots();
    
    // Process work queue
    process_work_queue();
}

void GarbageCollector::mark_roots() {
    // Scan stack
    scan_stack();
    
    // Scan globals (TODO: need linker support)
    // scan_globals();
    
    // Scan registers
    scan_registers();
    
    // Scan manual roots
    for (void* root : manual_roots_) {
        if (is_heap_pointer(root)) {
            add_to_work_queue(root);
        }
    }
}

void GarbageCollector::mark_object(void* ptr) {
    if (!ptr || !is_heap_pointer(ptr)) {
        return;
    }
    
    // Try to get header - this might fail if ptr is not actually an object
    try {
        ObjectHeader* header = Allocator::get_header(ptr);
        
        // Sanity check: size should be reasonable
        if (header->size == 0 || header->size > 1024 * 1024 * 1024) {
            return;  // Invalid object
        }
        
        // Already marked?
        if (header->is_marked()) {
            return;
        }
        
        // Mark it
        header->set_marked(true);
        stats_.objects_marked++;
        
        // TODO: Scan object for references
        // For now, we skip this to avoid false pointers
        // In the future, we'll use type information for precise scanning
        // scan_memory(ptr, header->size - sizeof(ObjectHeader));
    } catch (...) {
        // Invalid pointer, ignore
        return;
    }
}

void GarbageCollector::process_work_queue() {
    while (!work_queue_.empty()) {
        void* ptr = work_queue_.back();
        work_queue_.pop_back();
        mark_object(ptr);
    }
}

//=============================================================================
// Sweep Phase
//=============================================================================

void GarbageCollector::sweep() {
    stats_.objects_swept = 0;
    stats_.bytes_freed = 0;
    
    // Get all allocated objects from allocator
    // For now, we'll iterate through size classes
    // In a real implementation, we'd maintain a list of all objects
    
    // Note: This is a simplified implementation
    // A production GC would maintain a proper object list
    
    // For now, just clear mark bits
    // The actual sweeping will be done when we have a proper object list
}

//=============================================================================
// Root Scanning
//=============================================================================

void GarbageCollector::scan_stack() {
    // Get current stack pointer
    void* current_sp = get_stack_pointer();
    
    // Scan from current SP to stack bottom
    // Stack grows downward, so current_sp < stack_bottom_
    if (current_sp > stack_bottom_) {
        std::swap(current_sp, stack_bottom_);
    }
    
    scan_memory(current_sp, static_cast<char*>(stack_bottom_) - static_cast<char*>(current_sp));
}

void GarbageCollector::scan_globals() {
    // TODO: Scan global variables
    // This requires linker support to get data section bounds
    // For now, we rely on manual roots
}

void GarbageCollector::scan_registers() {
    // Save all registers to stack and scan them
    jmp_buf regs;
    setjmp(regs);
    scan_memory(&regs, sizeof(regs));
}

void GarbageCollector::scan_memory(void* start, size_t size) {
    // Conservative scanning: treat every word as a potential pointer
    void** ptr = static_cast<void**>(start);
    void** end = reinterpret_cast<void**>(static_cast<char*>(start) + size);
    
    for (; ptr < end; ptr++) {
        void* potential_ptr = *ptr;
        if (is_heap_pointer(potential_ptr)) {
            add_to_work_queue(potential_ptr);
        }
    }
}

//=============================================================================
// Helpers
//=============================================================================

bool GarbageCollector::is_heap_pointer(void* ptr) const {
    // Null check
    if (!ptr) {
        return false;
    }
    
    // Check if pointer is in reasonable range
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    if (addr < 0x1000 || addr > 0x7FFFFFFFFFFF) {
        return false;
    }
    
    // Check if pointer is aligned
    if (addr % sizeof(void*) != 0) {
        return false;
    }
    
    // For now, we can't reliably determine if it's a heap pointer
    // without maintaining a list of all allocated objects
    // So we'll be very conservative and only accept pointers
    // that we've explicitly added as roots
    return manual_roots_.count(ptr) > 0;
}

void GarbageCollector::add_to_work_queue(void* ptr) {
    if (ptr && is_heap_pointer(ptr)) {
        work_queue_.push_back(ptr);
    }
}

void GarbageCollector::add_root(void* ptr) {
    if (ptr) {
        manual_roots_.insert(ptr);
    }
}

void GarbageCollector::remove_root(void* ptr) {
    manual_roots_.erase(ptr);
}

//=============================================================================
// Initialization
//=============================================================================

void init_gc(Allocator* allocator) {
    if (!global_gc) {
        global_gc = new GarbageCollector(allocator);
    }
}

void shutdown_gc() {
    if (global_gc) {
        delete global_gc;
        global_gc = nullptr;
    }
}

} // namespace runtime
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

extern "C" {

void sapphire_gc_collect() {
    if (sapphire::runtime::global_gc) {
        sapphire::runtime::global_gc->collect();
    }
}

size_t sapphire_gc_collections() {
    if (sapphire::runtime::global_gc) {
        return sapphire::runtime::global_gc->stats().collections;
    }
    return 0;
}

size_t sapphire_gc_last_pause_us() {
    if (sapphire::runtime::global_gc) {
        return sapphire::runtime::global_gc->stats().last_pause_us;
    }
    return 0;
}

void sapphire_gc_add_root(void* ptr) {
    if (sapphire::runtime::global_gc) {
        sapphire::runtime::global_gc->add_root(ptr);
    }
}

void sapphire_gc_remove_root(void* ptr) {
    if (sapphire::runtime::global_gc) {
        sapphire::runtime::global_gc->remove_root(ptr);
    }
}

} // extern "C"
