#include "ownership.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace sapphire {
namespace runtime {

// Static members
std::unordered_map<void*, OwnershipInfo> OwnershipTracker::ownership_map_;
std::mutex OwnershipTracker::mutex_;

//=============================================================================
// Error Messages
//=============================================================================

static std::string ptr_to_string(void* ptr) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0')
        << reinterpret_cast<uintptr_t>(ptr);
    return oss.str();
}

UseAfterMoveError::UseAfterMoveError(void* ptr)
    : OwnershipError("Use-after-move detected: accessing moved object at " + ptr_to_string(ptr) +
                    "\nThis object was moved and is no longer valid." +
                    "\nDo not use an object after it has been moved.") {
}

OwnershipViolationError::OwnershipViolationError(void* ptr, uint64_t expected, uint64_t actual)
    : OwnershipError("Ownership violation detected at " + ptr_to_string(ptr) +
                    "\nExpected owner: " + std::to_string(expected) +
                    ", actual owner: " + std::to_string(actual) +
                    "\nCannot access object owned by another thread/context.") {
}

//=============================================================================
// OwnershipTracker Implementation
//=============================================================================

uint64_t OwnershipTracker::current_thread_id() {
    std::hash<std::thread::id> hasher;
    return hasher(std::this_thread::get_id());
}

void OwnershipTracker::set_owner(void* ptr, uint64_t owner_id) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    OwnershipInfo& info = ownership_map_[ptr];
    info.owner_id = owner_id;
    info.is_moved = false;
    info.move_count = 0;
}

uint64_t OwnershipTracker::get_owner(void* ptr) {
    if (!ptr) return 0;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        return it->second.owner_id;
    }
    return 0;
}

bool OwnershipTracker::is_owner(void* ptr, uint64_t owner_id) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        return it->second.owner_id == owner_id;
    }
    return false;
}

bool OwnershipTracker::has_owner(void* ptr) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    return ownership_map_.find(ptr) != ownership_map_.end();
}

void OwnershipTracker::mark_moved(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        it->second.is_moved = true;
        it->second.move_count++;
    }
}

bool OwnershipTracker::is_moved(void* ptr) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        return it->second.is_moved;
    }
    return false;
}

void OwnershipTracker::transfer_ownership(void* ptr, uint64_t new_owner) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        it->second.owner_id = new_owner;
        it->second.is_moved = false;  // Reset moved flag after transfer
        it->second.move_count++;
    } else {
        // Create new ownership info
        OwnershipInfo info;
        info.owner_id = new_owner;
        info.is_moved = false;
        info.move_count = 1;
        ownership_map_[ptr] = info;
    }
}

uint32_t OwnershipTracker::get_move_count(void* ptr) {
    if (!ptr) return 0;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        return it->second.move_count;
    }
    return 0;
}

void OwnershipTracker::mark_shared(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    OwnershipInfo& info = ownership_map_[ptr];
    info.is_shared = true;
    info.owner_id = 0;  // Shared objects have no single owner
}

bool OwnershipTracker::is_shared(void* ptr) {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        return it->second.is_shared;
    }
    return false;
}

void OwnershipTracker::untrack(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    ownership_map_.erase(ptr);
}

void OwnershipTracker::check_ownership(void* ptr, uint64_t owner_id) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        // Shared objects can be accessed by anyone
        if (it->second.is_shared) {
            return;
        }
        
        // Check ownership
        if (it->second.owner_id != owner_id) {
            throw OwnershipViolationError(ptr, owner_id, it->second.owner_id);
        }
    }
}

void OwnershipTracker::check_not_moved(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = ownership_map_.find(ptr);
    if (it != ownership_map_.end()) {
        if (it->second.is_moved) {
            throw UseAfterMoveError(ptr);
        }
    }
}

void OwnershipTracker::check_can_access(void* ptr, uint64_t accessor_id) {
    if (!ptr) return;
    
    // Check not moved
    check_not_moved(ptr);
    
    // Check ownership (will pass for shared objects)
    check_ownership(ptr, accessor_id);
}

size_t OwnershipTracker::tracked_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return ownership_map_.size();
}

size_t OwnershipTracker::moved_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(ownership_map_.begin(), ownership_map_.end(),
                        [](const auto& pair) { return pair.second.is_moved; });
}

size_t OwnershipTracker::shared_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(ownership_map_.begin(), ownership_map_.end(),
                        [](const auto& pair) { return pair.second.is_shared; });
}

void OwnershipTracker::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    ownership_map_.clear();
}

} // namespace runtime
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

extern "C" {

void sapphire_set_owner(void* ptr, uint64_t owner_id) {
    sapphire::runtime::OwnershipTracker::set_owner(ptr, owner_id);
}

uint64_t sapphire_get_owner(void* ptr) {
    return sapphire::runtime::OwnershipTracker::get_owner(ptr);
}

bool sapphire_is_owner(void* ptr, uint64_t owner_id) {
    return sapphire::runtime::OwnershipTracker::is_owner(ptr, owner_id);
}

void sapphire_mark_moved(void* ptr) {
    sapphire::runtime::OwnershipTracker::mark_moved(ptr);
}

bool sapphire_is_moved(void* ptr) {
    return sapphire::runtime::OwnershipTracker::is_moved(ptr);
}

void sapphire_transfer_ownership(void* ptr, uint64_t new_owner) {
    sapphire::runtime::OwnershipTracker::transfer_ownership(ptr, new_owner);
}

void sapphire_mark_shared(void* ptr) {
    sapphire::runtime::OwnershipTracker::mark_shared(ptr);
}

bool sapphire_is_shared(void* ptr) {
    return sapphire::runtime::OwnershipTracker::is_shared(ptr);
}

void sapphire_check_ownership(void* ptr, uint64_t owner_id) {
    sapphire::runtime::OwnershipTracker::check_ownership(ptr, owner_id);
}

void sapphire_check_not_moved(void* ptr) {
    sapphire::runtime::OwnershipTracker::check_not_moved(ptr);
}

uint64_t sapphire_current_thread_id() {
    return sapphire::runtime::OwnershipTracker::current_thread_id();
}

} // extern "C"
