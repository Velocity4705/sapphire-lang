#include "refcount.h"
#include <iostream>

namespace sapphire {
namespace runtime {

//=============================================================================
// ControlBlock Implementation
//=============================================================================

ControlBlock::ControlBlock(void* data, std::function<void(void*)> deleter)
    : strong_count(1), weak_count(0), data(data), deleter(std::move(deleter)) {
}

ControlBlock::~ControlBlock() {
    // Control block is deleted when both strong and weak counts reach 0
}

void ControlBlock::inc_strong() {
    strong_count.fetch_add(1, std::memory_order_relaxed);
}

void ControlBlock::dec_strong() {
    uint32_t old_count = strong_count.fetch_sub(1, std::memory_order_acq_rel);
    
    if (old_count == 1) {
        // Last strong reference dropped - delete the object
        if (deleter && data) {
            deleter(data);
        }
        data = nullptr;
        
        // If no weak references, delete control block
        if (weak_count.load(std::memory_order_acquire) == 0) {
            delete this;
        }
    }
}

void ControlBlock::inc_weak() {
    weak_count.fetch_add(1, std::memory_order_relaxed);
}

void ControlBlock::dec_weak() {
    uint32_t old_count = weak_count.fetch_sub(1, std::memory_order_acq_rel);
    
    if (old_count == 1) {
        // Last weak reference dropped
        // If object already deleted, delete control block
        if (strong_count.load(std::memory_order_acquire) == 0) {
            delete this;
        }
    }
}

uint32_t ControlBlock::get_strong_count() const {
    return strong_count.load(std::memory_order_relaxed);
}

uint32_t ControlBlock::get_weak_count() const {
    return weak_count.load(std::memory_order_relaxed);
}

bool ControlBlock::is_alive() const {
    return strong_count.load(std::memory_order_relaxed) > 0;
}

} // namespace runtime
} // namespace sapphire
