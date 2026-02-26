#ifndef SAPPHIRE_RUNTIME_RWLOCK_H
#define SAPPHIRE_RUNTIME_RWLOCK_H

#include <shared_mutex>

namespace sapphire {
namespace runtime {

/**
 * RwLock - Reader-Writer lock
 * 
 * Allows multiple readers or one writer.
 */
class RwLock {
private:
    std::shared_mutex mutex_;
    
public:
    RwLock() = default;
    ~RwLock() = default;
    
    // Delete copy and move operations
    RwLock(const RwLock&) = delete;
    RwLock& operator=(const RwLock&) = delete;
    RwLock(RwLock&&) = delete;
    RwLock& operator=(RwLock&&) = delete;
    
    // Read lock operations
    void read_lock();
    bool try_read_lock();
    void read_unlock();
    
    // Write lock operations
    void write_lock();
    bool try_write_lock();
    void write_unlock();
    
    // RAII guards
    class ReadGuard {
    private:
        RwLock& lock_;
        bool locked_;
        
    public:
        explicit ReadGuard(RwLock& lock);
        ~ReadGuard();
        
        ReadGuard(const ReadGuard&) = delete;
        ReadGuard& operator=(const ReadGuard&) = delete;
        ReadGuard(ReadGuard&&) = delete;
        ReadGuard& operator=(ReadGuard&&) = delete;
    };
    
    class WriteGuard {
    private:
        RwLock& lock_;
        bool locked_;
        
    public:
        explicit WriteGuard(RwLock& lock);
        ~WriteGuard();
        
        WriteGuard(const WriteGuard&) = delete;
        WriteGuard& operator=(const WriteGuard&) = delete;
        WriteGuard(WriteGuard&&) = delete;
        WriteGuard& operator=(WriteGuard&&) = delete;
    };
    
    ReadGuard read_guard();
    WriteGuard write_guard();
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_RUNTIME_RWLOCK_H
