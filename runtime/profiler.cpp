#include "profiler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>

namespace sapphire {
namespace runtime {

// Static members
std::unordered_map<void*, AllocationInfo> AllocationTracker::allocations_;
std::mutex AllocationTracker::mutex_;
size_t AllocationTracker::total_allocated_ = 0;
size_t AllocationTracker::total_freed_ = 0;
size_t AllocationTracker::peak_usage_ = 0;

bool MemoryProfiler::enabled_ = false;
std::mutex MemoryProfiler::mutex_;

//=============================================================================
// Helper Functions
//=============================================================================

static std::string format_bytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

static std::string ptr_to_string(void* ptr) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0')
        << reinterpret_cast<uintptr_t>(ptr);
    return oss.str();
}

//=============================================================================
// AllocationTracker Implementation
//=============================================================================

void AllocationTracker::track_allocation(void* ptr, size_t size, uint64_t type_id) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    AllocationInfo info(ptr, size, type_id);
    allocations_[ptr] = info;
    
    total_allocated_ += size;
    size_t current = total_allocated_ - total_freed_;
    if (current > peak_usage_) {
        peak_usage_ = current;
    }
}

void AllocationTracker::track_free(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        it->second.is_freed = true;
        total_freed_ += it->second.size;
    }
}

size_t AllocationTracker::total_allocated() {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_allocated_;
}

size_t AllocationTracker::total_freed() {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_freed_;
}

size_t AllocationTracker::current_usage() {
    std::lock_guard<std::mutex> lock(mutex_);
    return total_allocated_ - total_freed_;
}

size_t AllocationTracker::peak_usage() {
    std::lock_guard<std::mutex> lock(mutex_);
    return peak_usage_;
}

size_t AllocationTracker::allocation_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return allocations_.size();
}

size_t AllocationTracker::free_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(allocations_.begin(), allocations_.end(),
                        [](const auto& pair) { return pair.second.is_freed; });
}

size_t AllocationTracker::leak_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(allocations_.begin(), allocations_.end(),
                        [](const auto& pair) { return !pair.second.is_freed; });
}

std::vector<AllocationInfo> AllocationTracker::get_live_allocations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AllocationInfo> result;
    for (const auto& [ptr, info] : allocations_) {
        if (!info.is_freed) {
            result.push_back(info);
        }
    }
    return result;
}

std::vector<AllocationInfo> AllocationTracker::get_all_allocations() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AllocationInfo> result;
    for (const auto& [ptr, info] : allocations_) {
        result.push_back(info);
    }
    return result;
}

std::vector<AllocationInfo> AllocationTracker::get_leaks() {
    return get_live_allocations();  // Live allocations are potential leaks
}

bool AllocationTracker::is_tracked(void* ptr) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    return allocations_.find(ptr) != allocations_.end();
}

bool AllocationTracker::is_leaked(void* ptr) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocations_.find(ptr);
    return it != allocations_.end() && !it->second.is_freed;
}

AllocationInfo AllocationTracker::get_info(void* ptr) {
    if (!ptr) return AllocationInfo();
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        return it->second;
    }
    return AllocationInfo();
}

void AllocationTracker::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.clear();
    total_allocated_ = 0;
    total_freed_ = 0;
    peak_usage_ = 0;
}

//=============================================================================
// MemoryProfiler Implementation
//=============================================================================

void MemoryProfiler::enable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = true;
}

void MemoryProfiler::disable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = false;
}

bool MemoryProfiler::is_enabled() {
    std::lock_guard<std::mutex> lock(mutex_);
    return enabled_;
}

MemoryStats MemoryProfiler::get_stats() {
    MemoryStats stats;
    
    stats.total_allocated = AllocationTracker::total_allocated();
    stats.total_freed = AllocationTracker::total_freed();
    stats.current_usage = AllocationTracker::current_usage();
    stats.peak_usage = AllocationTracker::peak_usage();
    stats.allocation_count = AllocationTracker::allocation_count();
    stats.free_count = AllocationTracker::free_count();
    stats.leak_count = AllocationTracker::leak_count();
    
    // Group by size
    auto allocations = AllocationTracker::get_all_allocations();
    for (const auto& info : allocations) {
        stats.allocations_by_size[info.size]++;
        stats.allocations_by_type[info.type_id]++;
    }
    
    return stats;
}

void MemoryProfiler::print_stats() {
    MemoryStats stats = get_stats();
    
    std::cout << "================================================================================\n";
    std::cout << "Memory Profiler Statistics\n";
    std::cout << "================================================================================\n\n";
    
    std::cout << "Memory Usage:\n";
    std::cout << "  Total allocated:  " << format_bytes(stats.total_allocated) 
              << " (" << stats.total_allocated << " bytes)\n";
    std::cout << "  Total freed:      " << format_bytes(stats.total_freed)
              << " (" << stats.total_freed << " bytes)\n";
    std::cout << "  Current usage:    " << format_bytes(stats.current_usage)
              << " (" << stats.current_usage << " bytes)\n";
    std::cout << "  Peak usage:       " << format_bytes(stats.peak_usage)
              << " (" << stats.peak_usage << " bytes)\n";
    
    std::cout << "\nAllocation Counts:\n";
    std::cout << "  Total allocations: " << stats.allocation_count << "\n";
    std::cout << "  Freed:             " << stats.free_count << "\n";
    std::cout << "  Live:              " << (stats.allocation_count - stats.free_count) << "\n";
    std::cout << "  Leaks:             " << stats.leak_count << "\n";
    
    if (!stats.allocations_by_size.empty()) {
        std::cout << "\nAllocations by Size:\n";
        for (const auto& [size, count] : stats.allocations_by_size) {
            std::cout << "  " << std::setw(8) << format_bytes(size) 
                     << ": " << count << " allocations\n";
        }
    }
    
    if (!stats.allocations_by_type.empty()) {
        std::cout << "\nAllocations by Type:\n";
        for (const auto& [type_id, count] : stats.allocations_by_type) {
            std::cout << "  Type " << type_id << ": " << count << " allocations\n";
        }
    }
    
    std::cout << "\n================================================================================\n";
}

void MemoryProfiler::print_leaks() {
    auto leaks = AllocationTracker::get_leaks();
    
    std::cout << "================================================================================\n";
    std::cout << "Memory Leak Report\n";
    std::cout << "================================================================================\n\n";
    
    if (leaks.empty()) {
        std::cout << "✓ No memory leaks detected!\n";
    } else {
        std::cout << "✗ " << leaks.size() << " memory leak(s) detected:\n\n";
        
        size_t total_leaked = 0;
        for (const auto& info : leaks) {
            std::cout << "  Leak at " << ptr_to_string(info.ptr) << ":\n";
            std::cout << "    Size:      " << format_bytes(info.size) << "\n";
            std::cout << "    Type ID:   " << info.type_id << "\n";
            std::cout << "    Timestamp: " << info.timestamp << "\n";
            std::cout << "\n";
            total_leaked += info.size;
        }
        
        std::cout << "Total leaked: " << format_bytes(total_leaked) << "\n";
    }
    
    std::cout << "================================================================================\n";
}

void MemoryProfiler::print_summary() {
    MemoryStats stats = get_stats();
    
    std::cout << "Memory Summary: ";
    std::cout << format_bytes(stats.current_usage) << " used, ";
    std::cout << stats.leak_count << " leak(s)";
    
    if (stats.leak_count == 0) {
        std::cout << " ✓";
    } else {
        std::cout << " ✗";
    }
    std::cout << "\n";
}

void MemoryProfiler::reset() {
    AllocationTracker::clear();
}

bool MemoryProfiler::has_leaks() {
    return AllocationTracker::leak_count() > 0;
}

size_t MemoryProfiler::leak_count() {
    return AllocationTracker::leak_count();
}

//=============================================================================
// LeakDetector Implementation
//=============================================================================

std::vector<void*> LeakDetector::find_leaks() {
    auto leaks = AllocationTracker::get_leaks();
    
    std::vector<void*> result;
    for (const auto& info : leaks) {
        result.push_back(info.ptr);
    }
    return result;
}

bool LeakDetector::is_leaked(void* ptr) {
    return AllocationTracker::is_leaked(ptr);
}

AllocationInfo LeakDetector::get_leak_info(void* ptr) {
    return AllocationTracker::get_info(ptr);
}

void LeakDetector::print_leak_report() {
    MemoryProfiler::print_leaks();
}

size_t LeakDetector::leak_count() {
    return AllocationTracker::leak_count();
}

} // namespace runtime
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

extern "C" {

void sapphire_profiler_enable() {
    sapphire::runtime::MemoryProfiler::enable();
}

void sapphire_profiler_disable() {
    sapphire::runtime::MemoryProfiler::disable();
}

bool sapphire_profiler_is_enabled() {
    return sapphire::runtime::MemoryProfiler::is_enabled();
}

void sapphire_profiler_track_allocation(void* ptr, size_t size, uint64_t type_id) {
    if (sapphire::runtime::MemoryProfiler::is_enabled()) {
        sapphire::runtime::AllocationTracker::track_allocation(ptr, size, type_id);
    }
}

void sapphire_profiler_track_free(void* ptr) {
    if (sapphire::runtime::MemoryProfiler::is_enabled()) {
        sapphire::runtime::AllocationTracker::track_free(ptr);
    }
}

size_t sapphire_profiler_total_allocated() {
    return sapphire::runtime::AllocationTracker::total_allocated();
}

size_t sapphire_profiler_current_usage() {
    return sapphire::runtime::AllocationTracker::current_usage();
}

size_t sapphire_profiler_leak_count() {
    return sapphire::runtime::AllocationTracker::leak_count();
}

void sapphire_profiler_print_stats() {
    sapphire::runtime::MemoryProfiler::print_stats();
}

void sapphire_profiler_print_leaks() {
    sapphire::runtime::MemoryProfiler::print_leaks();
}

void sapphire_profiler_reset() {
    sapphire::runtime::MemoryProfiler::reset();
}

} // extern "C"
