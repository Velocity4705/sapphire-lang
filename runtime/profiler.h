#ifndef SAPPHIRE_PROFILER_H
#define SAPPHIRE_PROFILER_H

#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <map>
#include <vector>
#include <mutex>
#include <chrono>

namespace sapphire {
namespace runtime {

/**
 * Allocation Info
 * 
 * Information about a single allocation.
 */
struct AllocationInfo {
    void* ptr;
    size_t size;
    uint64_t type_id;
    uint64_t timestamp;
    bool is_freed;
    
    AllocationInfo()
        : ptr(nullptr), size(0), type_id(0), timestamp(0), is_freed(false) {}
    
    AllocationInfo(void* p, size_t s, uint64_t t)
        : ptr(p), size(s), type_id(t), 
          timestamp(std::chrono::steady_clock::now().time_since_epoch().count()),
          is_freed(false) {}
};

/**
 * Memory Statistics
 * 
 * Aggregate memory usage statistics.
 */
struct MemoryStats {
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t free_count;
    size_t leak_count;
    
    // Allocations by size
    std::map<size_t, size_t> allocations_by_size;
    
    // Allocations by type
    std::map<uint64_t, size_t> allocations_by_type;
    
    MemoryStats()
        : total_allocated(0), total_freed(0), current_usage(0), peak_usage(0),
          allocation_count(0), free_count(0), leak_count(0) {}
};

/**
 * Allocation Tracker
 * 
 * Tracks all allocations and frees for profiling.
 */
class AllocationTracker {
private:
    static std::unordered_map<void*, AllocationInfo> allocations_;
    static std::mutex mutex_;
    static size_t total_allocated_;
    static size_t total_freed_;
    static size_t peak_usage_;
    
public:
    // Track allocation
    static void track_allocation(void* ptr, size_t size, uint64_t type_id);
    
    // Track free
    static void track_free(void* ptr);
    
    // Statistics
    static size_t total_allocated();
    static size_t total_freed();
    static size_t current_usage();
    static size_t peak_usage();
    static size_t allocation_count();
    static size_t free_count();
    static size_t leak_count();
    
    // Get allocations
    static std::vector<AllocationInfo> get_live_allocations();
    static std::vector<AllocationInfo> get_all_allocations();
    static std::vector<AllocationInfo> get_leaks();
    
    // Check if pointer is tracked
    static bool is_tracked(void* ptr);
    static bool is_leaked(void* ptr);
    
    // Get allocation info
    static AllocationInfo get_info(void* ptr);
    
    // Clear all tracking
    static void clear();
};

/**
 * Memory Profiler
 * 
 * High-level interface for memory profiling.
 */
class MemoryProfiler {
private:
    static bool enabled_;
    static std::mutex mutex_;
    
public:
    // Enable/disable profiler
    static void enable();
    static void disable();
    static bool is_enabled();
    
    // Get statistics
    static MemoryStats get_stats();
    
    // Print reports
    static void print_stats();
    static void print_leaks();
    static void print_summary();
    
    // Reset profiler
    static void reset();
    
    // Check for leaks
    static bool has_leaks();
    static size_t leak_count();
};

/**
 * Leak Detector
 * 
 * Detects and reports memory leaks.
 */
class LeakDetector {
public:
    // Find leaks
    static std::vector<void*> find_leaks();
    
    // Check if pointer is leaked
    static bool is_leaked(void* ptr);
    
    // Get leak info
    static AllocationInfo get_leak_info(void* ptr);
    
    // Print leak report
    static void print_leak_report();
    
    // Get leak count
    static size_t leak_count();
};

} // namespace runtime
} // namespace sapphire

// C API for use from generated code
extern "C" {
    // Enable/disable profiler
    void sapphire_profiler_enable();
    void sapphire_profiler_disable();
    bool sapphire_profiler_is_enabled();
    
    // Track allocation/free
    void sapphire_profiler_track_allocation(void* ptr, size_t size, uint64_t type_id);
    void sapphire_profiler_track_free(void* ptr);
    
    // Statistics
    size_t sapphire_profiler_total_allocated();
    size_t sapphire_profiler_current_usage();
    size_t sapphire_profiler_leak_count();
    
    // Reports
    void sapphire_profiler_print_stats();
    void sapphire_profiler_print_leaks();
    
    // Reset
    void sapphire_profiler_reset();
}

#endif // SAPPHIRE_PROFILER_H
