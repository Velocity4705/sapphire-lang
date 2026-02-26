#include "safety.h"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace sapphire {
namespace runtime {

// Poison pattern for freed memory
static constexpr uint8_t POISON_PATTERN = 0xDD;

//=============================================================================
// Error Messages
//=============================================================================

static std::string ptr_to_string(void* ptr) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(16) << std::setfill('0')
        << reinterpret_cast<uintptr_t>(ptr);
    return oss.str();
}

UseAfterFreeError::UseAfterFreeError(void* ptr)
    : SafetyError("Use-after-free detected: accessing freed memory at " + ptr_to_string(ptr) +
                 "\nThis memory was previously freed and is no longer valid.") {
}

DoubleFreeError::DoubleFreeError(void* ptr)
    : SafetyError("Double-free detected: attempting to free already freed memory at " + ptr_to_string(ptr) +
                 "\nThis memory was already freed once.") {
}

BufferOverflowError::BufferOverflowError(size_t index, size_t length)
    : SafetyError("Buffer overflow detected: index " + std::to_string(index) +
                 " is out of bounds (length: " + std::to_string(length) + ")" +
                 "\nValid indices are 0 to " + std::to_string(length - 1) + ".") {
}

TypeMismatchError::TypeMismatchError(uint64_t actual, uint64_t expected)
    : SafetyError("Type mismatch detected: expected type " + std::to_string(expected) +
                 ", but got type " + std::to_string(actual) +
                 "\nCannot cast between incompatible types.") {
}

NullPointerError::NullPointerError()
    : SafetyError("Null pointer dereference detected: attempting to access null pointer" +
                 std::string("\nMake sure the pointer is initialized before use.")) {
}

//=============================================================================
// SafetyChecker Implementation
//=============================================================================

bool SafetyChecker::is_valid_pointer(void* ptr) {
    if (!ptr) {
        return false;
    }
    
    // Check if it looks like a heap pointer
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    if (addr < 0x1000 || addr > 0x7FFFFFFFFFFF) {
        return false;
    }
    
    // Check alignment
    if (addr % sizeof(void*) != 0) {
        return false;
    }
    
    return true;
}

bool SafetyChecker::is_freed(void* ptr) {
    if (!is_valid_pointer(ptr)) {
        return false;
    }
    
    try {
        ObjectHeader* header = Allocator::get_header(ptr);
        return header->is_freed();
    } catch (...) {
        return false;
    }
}

bool SafetyChecker::is_null(void* ptr) {
    return ptr == nullptr;
}

void SafetyChecker::check_bounds(size_t index, size_t length) {
    if (index >= length) {
        throw BufferOverflowError(index, length);
    }
}

void SafetyChecker::check_type(void* ptr, uint64_t expected_type) {
    if (!ptr) {
        throw NullPointerError();
    }
    
    if (!is_valid_pointer(ptr)) {
        throw SafetyError("Invalid pointer in type check");
    }
    
    try {
        ObjectHeader* header = Allocator::get_header(ptr);
        if (header->type_id != expected_type) {
            throw TypeMismatchError(header->type_id, expected_type);
        }
    } catch (const SafetyError&) {
        throw;
    } catch (...) {
        throw SafetyError("Failed to check type: invalid pointer");
    }
}

void* SafetyChecker::check_null(void* ptr) {
    if (!ptr) {
        throw NullPointerError();
    }
    return ptr;
}

void* SafetyChecker::check_use_after_free(void* ptr) {
    if (!ptr) {
        return ptr;  // Null is handled by check_null
    }
    
    if (!is_valid_pointer(ptr)) {
        throw SafetyError("Invalid pointer");
    }
    
    try {
        ObjectHeader* header = Allocator::get_header(ptr);
        if (header->is_freed()) {
            throw UseAfterFreeError(ptr);
        }
    } catch (const SafetyError&) {
        throw;
    } catch (...) {
        // If we can't get the header, assume it's invalid
        throw SafetyError("Invalid pointer in use-after-free check");
    }
    
    return ptr;
}

void SafetyChecker::mark_freed(void* ptr) {
    if (!ptr || !is_valid_pointer(ptr)) {
        return;
    }
    
    try {
        ObjectHeader* header = Allocator::get_header(ptr);
        
        // Check for double-free
        if (header->is_freed()) {
            throw DoubleFreeError(ptr);
        }
        
        // Mark as freed
        header->set_freed(true);
        
        // Note: We don't poison memory here because the allocator's free list
        // uses the freed memory to store FreeNode structures. Poisoning would
        // corrupt the free list. In a production system, we could:
        // 1. Use a separate free list structure (more memory overhead)
        // 2. Only poison on actual deallocation, not on free list insertion
        // 3. Use guard pages or other techniques
        // For now, we rely on the FREED_BIT flag for use-after-free detection.
    } catch (const SafetyError&) {
        throw;
    } catch (...) {
        // Ignore errors in marking
    }
}

void SafetyChecker::poison_memory(void* ptr, size_t size) {
    if (ptr && size > 0) {
        std::memset(ptr, POISON_PATTERN, size);
    }
}

bool SafetyChecker::is_poisoned(void* ptr, size_t size) {
    if (!ptr || size == 0) {
        return false;
    }
    
    uint8_t* bytes = static_cast<uint8_t*>(ptr);
    for (size_t i = 0; i < size; i++) {
        if (bytes[i] != POISON_PATTERN) {
            return false;
        }
    }
    return true;
}

} // namespace runtime
} // namespace sapphire

//=============================================================================
// C API
//=============================================================================

extern "C" {

bool sapphire_is_valid_pointer(void* ptr) {
    return sapphire::runtime::SafetyChecker::is_valid_pointer(ptr);
}

bool sapphire_is_freed(void* ptr) {
    return sapphire::runtime::SafetyChecker::is_freed(ptr);
}

void sapphire_check_bounds(size_t index, size_t length) {
    sapphire::runtime::SafetyChecker::check_bounds(index, length);
}

void sapphire_check_type(void* ptr, uint64_t expected_type) {
    sapphire::runtime::SafetyChecker::check_type(ptr, expected_type);
}

void* sapphire_check_null(void* ptr) {
    return sapphire::runtime::SafetyChecker::check_null(ptr);
}

void* sapphire_check_use_after_free(void* ptr) {
    return sapphire::runtime::SafetyChecker::check_use_after_free(ptr);
}

void sapphire_mark_freed(void* ptr) {
    sapphire::runtime::SafetyChecker::mark_freed(ptr);
}

} // extern "C"
