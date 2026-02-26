#ifndef SAPPHIRE_RUNTIME_MUTEX_H
#define SAPPHIRE_RUNTIME_MUTEX_H

#include <mutex>

namespace sapphire {
namespace runtime {

/**
 * Mutex - Mutual exclusion lock
 * 
 * Provides thread-safe mutual exclusion.
 */
class Mutex {
private:
    std::mutex mutex_;
    
public:
    Mutex() = default;
    ~Mutex() = default;
    
    // Delete copy and move operations
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
    Mutex& operator=(Mutex&&) = delete;
    
    // Lock operations
    void lock();
    bool try_lock();
    void unlock();
    
    // RAII guard
    class Guard {
    private:
        Mutex& mutex_;
        bool locked_;
        
    public:
        explicit Guard(Mutex& m);
        ~Guard();
        
        // Delete copy and move
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;
        Guard(Guard&&) = delete;
        Guard& operator=(Guard&&) = delete;
    };
    
    Guard lock_guard();
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_RUNTIME_MUTEX_H
