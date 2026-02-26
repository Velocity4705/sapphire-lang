#ifndef SAPPHIRE_REFCOUNT_H
#define SAPPHIRE_REFCOUNT_H

#include <atomic>
#include <cstdint>
#include <utility>
#include <functional>

namespace sapphire {
namespace runtime {

/**
 * Control Block
 * 
 * Stores reference counts and manages object lifetime.
 * Separate from the object to allow weak references to outlive the object.
 */
struct ControlBlock {
    std::atomic<uint32_t> strong_count;
    std::atomic<uint32_t> weak_count;
    void* data;
    std::function<void(void*)> deleter;
    
    ControlBlock(void* data, std::function<void(void*)> deleter);
    ~ControlBlock();
    
    void inc_strong();
    void dec_strong();
    void inc_weak();
    void dec_weak();
    
    uint32_t get_strong_count() const;
    uint32_t get_weak_count() const;
    bool is_alive() const;
};

/**
 * Rc<T> - Reference Counted Pointer
 * 
 * Single-threaded reference counted smart pointer.
 * Provides deterministic cleanup when the last reference is dropped.
 * Use for resources that need immediate cleanup (files, sockets, etc.)
 */
template<typename T>
class Rc {
private:
    ControlBlock* control_;
    
public:
    // Default constructor (null)
    Rc() : control_(nullptr) {}
    
    // Construct from control block (for Weak upgrade)
    explicit Rc(ControlBlock* control) : control_(control) {
        if (control_) {
            control_->inc_strong();
        }
    }
    
    // Construct from raw pointer
    explicit Rc(T* ptr) : control_(nullptr) {
        if (ptr) {
            control_ = new ControlBlock(
                ptr,
                [](void* p) { delete static_cast<T*>(p); }
            );
        }
    }
    
    // Copy constructor
    Rc(const Rc& other) : control_(other.control_) {
        if (control_) {
            control_->inc_strong();
        }
    }
    
    // Move constructor
    Rc(Rc&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    // Destructor
    ~Rc() {
        if (control_) {
            control_->dec_strong();
        }
    }
    
    // Copy assignment
    Rc& operator=(const Rc& other) {
        if (this != &other) {
            if (control_) {
                control_->dec_strong();
            }
            control_ = other.control_;
            if (control_) {
                control_->inc_strong();
            }
        }
        return *this;
    }
    
    // Move assignment
    Rc& operator=(Rc&& other) noexcept {
        if (this != &other) {
            if (control_) {
                control_->dec_strong();
            }
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // Get raw pointer
    T* get() const {
        return control_ && control_->is_alive() 
            ? static_cast<T*>(control_->data) 
            : nullptr;
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
        return get() != nullptr;
    }
    
    // Get reference count
    uint32_t use_count() const {
        return control_ ? control_->get_strong_count() : 0;
    }
    
    // Clone (explicit copy)
    Rc clone() const {
        return Rc(*this);
    }
    
    // Reset to null
    void reset() {
        if (control_) {
            control_->dec_strong();
            control_ = nullptr;
        }
    }
    
    // Get control block (for Weak)
    ControlBlock* get_control() const {
        return control_;
    }
    
    // Factory method
    template<typename... Args>
    static Rc<T> make(Args&&... args) {
        return Rc<T>(new T(std::forward<Args>(args)...));
    }
};

/**
 * Arc<T> - Atomic Reference Counted Pointer
 * 
 * Thread-safe reference counted smart pointer.
 * Uses atomic operations for thread safety.
 * Can be shared across threads safely.
 */
template<typename T>
class Arc {
private:
    ControlBlock* control_;
    
public:
    // Default constructor (null)
    Arc() : control_(nullptr) {}
    
    // Construct from control block (for Weak upgrade)
    explicit Arc(ControlBlock* control) : control_(control) {
        if (control_) {
            control_->inc_strong();
        }
    }
    
    // Construct from raw pointer
    explicit Arc(T* ptr) : control_(nullptr) {
        if (ptr) {
            control_ = new ControlBlock(
                ptr,
                [](void* p) { delete static_cast<T*>(p); }
            );
        }
    }
    
    // Copy constructor (thread-safe)
    Arc(const Arc& other) : control_(other.control_) {
        if (control_) {
            control_->inc_strong();
        }
    }
    
    // Move constructor
    Arc(Arc&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    // Destructor
    ~Arc() {
        if (control_) {
            control_->dec_strong();
        }
    }
    
    // Copy assignment (thread-safe)
    Arc& operator=(const Arc& other) {
        if (this != &other) {
            if (control_) {
                control_->dec_strong();
            }
            control_ = other.control_;
            if (control_) {
                control_->inc_strong();
            }
        }
        return *this;
    }
    
    // Move assignment
    Arc& operator=(Arc&& other) noexcept {
        if (this != &other) {
            if (control_) {
                control_->dec_strong();
            }
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // Get raw pointer
    T* get() const {
        return control_ && control_->is_alive() 
            ? static_cast<T*>(control_->data) 
            : nullptr;
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
        return get() != nullptr;
    }
    
    // Get reference count
    uint32_t use_count() const {
        return control_ ? control_->get_strong_count() : 0;
    }
    
    // Clone (explicit copy)
    Arc clone() const {
        return Arc(*this);
    }
    
    // Reset to null
    void reset() {
        if (control_) {
            control_->dec_strong();
            control_ = nullptr;
        }
    }
    
    // Get control block (for Weak)
    ControlBlock* get_control() const {
        return control_;
    }
    
    // Factory method
    template<typename... Args>
    static Arc<T> make(Args&&... args) {
        return Arc<T>(new T(std::forward<Args>(args)...));
    }
};

/**
 * Weak<T> - Weak Reference
 * 
 * Non-owning reference that doesn't prevent object destruction.
 * Used to break reference cycles.
 * Must be upgraded to Rc/Arc before use.
 */
template<typename T>
class Weak {
private:
    ControlBlock* control_;
    
public:
    // Construct from Rc
    explicit Weak(const Rc<T>& rc) : control_(rc.get_control()) {
        if (control_) {
            control_->inc_weak();
        }
    }
    
    // Construct from Arc
    explicit Weak(const Arc<T>& arc) : control_(arc.get_control()) {
        if (control_) {
            control_->inc_weak();
        }
    }
    
    // Copy constructor
    Weak(const Weak& other) : control_(other.control_) {
        if (control_) {
            control_->inc_weak();
        }
    }
    
    // Move constructor
    Weak(Weak&& other) noexcept : control_(other.control_) {
        other.control_ = nullptr;
    }
    
    // Destructor
    ~Weak() {
        if (control_) {
            control_->dec_weak();
        }
    }
    
    // Copy assignment
    Weak& operator=(const Weak& other) {
        if (this != &other) {
            if (control_) {
                control_->dec_weak();
            }
            control_ = other.control_;
            if (control_) {
                control_->inc_weak();
            }
        }
        return *this;
    }
    
    // Move assignment
    Weak& operator=(Weak&& other) noexcept {
        if (this != &other) {
            if (control_) {
                control_->dec_weak();
            }
            control_ = other.control_;
            other.control_ = nullptr;
        }
        return *this;
    }
    
    // Upgrade to Rc (returns null if expired)
    Rc<T> upgrade_rc() const {
        if (control_ && control_->is_alive()) {
            return Rc<T>(control_);
        }
        return Rc<T>(static_cast<T*>(nullptr));
    }
    
    // Upgrade to Arc (returns null if expired)
    Arc<T> upgrade_arc() const {
        if (control_ && control_->is_alive()) {
            return Arc<T>(control_);
        }
        return Arc<T>(static_cast<T*>(nullptr));
    }
    
    // Check if expired
    bool expired() const {
        return !control_ || !control_->is_alive();
    }
    
    // Get weak count
    uint32_t weak_count() const {
        return control_ ? control_->get_weak_count() : 0;
    }
    
    // Get strong count
    uint32_t use_count() const {
        return control_ ? control_->get_strong_count() : 0;
    }
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_REFCOUNT_H
