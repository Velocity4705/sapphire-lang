#ifndef SAPPHIRE_GC_H
#define SAPPHIRE_GC_H

#include "allocator.h"
#include <vector>
#include <set>
#include <cstdint>
#include <csetjmp>

namespace sapphire {
namespace runtime {

/**
 * GC Phase
 * 
 * Current phase of garbage collection.
 */
enum class GCPhase {
    IDLE,       // Not running
    MARKING,    // Mark phase (finding reachable objects)
    SWEEPING    // Sweep phase (freeing unreachable objects)
};

/**
 * GC Statistics
 * 
 * Statistics about garbage collection.
 */
struct GCStats {
    size_t collections;          // Number of GC cycles
    size_t objects_marked;       // Objects marked in last GC
    size_t objects_swept;        // Objects swept in last GC
    size_t bytes_freed;          // Bytes freed in last GC
    uint64_t last_pause_us;      // Last GC pause time (microseconds)
    uint64_t total_pause_us;     // Total GC pause time
    size_t peak_memory;          // Peak memory usage
};

/**
 * Garbage Collector
 * 
 * Mark-and-sweep garbage collector for Sapphire.
 * 
 * Algorithm:
 * 1. Mark Phase: Trace from roots, mark all reachable objects
 * 2. Sweep Phase: Free all unmarked objects
 * 
 * Roots:
 * - Stack variables
 * - Global variables
 * - CPU registers
 */
class GarbageCollector {
public:
    GarbageCollector(Allocator* allocator);
    ~GarbageCollector();
    
    // Run garbage collection
    void collect();
    
    // Check if GC should run
    bool should_collect() const;
    
    // Add root manually
    void add_root(void* ptr);
    
    // Remove root
    void remove_root(void* ptr);
    
    // Get statistics
    const GCStats& stats() const { return stats_; }
    
    // Get current phase
    GCPhase phase() const { return phase_; }
    
    // Configuration
    void set_threshold(size_t bytes) { gc_threshold_ = bytes; }
    size_t threshold() const { return gc_threshold_; }
    
private:
    // Mark phase
    void mark();
    void mark_roots();
    void mark_object(void* ptr);
    void process_work_queue();
    
    // Sweep phase
    void sweep();
    
    // Root scanning
    void scan_stack();
    void scan_globals();
    void scan_registers();
    void scan_memory(void* start, size_t size);
    
    // Helpers
    bool is_heap_pointer(void* ptr) const;
    void add_to_work_queue(void* ptr);
    
    // State
    Allocator* allocator_;
    GCPhase phase_;
    
    // Work queue for marking
    std::vector<void*> work_queue_;
    
    // Manual roots
    std::set<void*> manual_roots_;
    
    // Stack bounds (for conservative scanning)
    void* stack_bottom_;
    void* stack_top_;
    
    // Heap bounds (for pointer validation)
    void* heap_start_;
    void* heap_end_;
    
    // GC trigger threshold
    size_t gc_threshold_;
    size_t bytes_at_last_gc_;
    
    // Statistics
    GCStats stats_;
    
    // Timing
    uint64_t gc_start_time_;
};

// Global GC instance
extern GarbageCollector* global_gc;

// Initialize GC
void init_gc(Allocator* allocator);

// Shutdown GC
void shutdown_gc();

} // namespace runtime
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Trigger garbage collection
    void sapphire_gc_collect();
    
    // Get GC statistics
    size_t sapphire_gc_collections();
    size_t sapphire_gc_last_pause_us();
    
    // Add/remove roots
    void sapphire_gc_add_root(void* ptr);
    void sapphire_gc_remove_root(void* ptr);
}

#endif // SAPPHIRE_GC_H
