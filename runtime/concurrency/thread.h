#ifndef SAPPHIRE_RUNTIME_THREAD_H
#define SAPPHIRE_RUNTIME_THREAD_H

#include <thread>
#include <string>
#include <functional>
#include <memory>
#include <chrono>

namespace sapphire {
namespace runtime {

/**
 * Thread - Lightweight thread abstraction
 * 
 * Provides a simple interface for spawning and managing threads.
 */
class Thread {
private:
    std::unique_ptr<std::thread> thread_;
    std::string name_;
    uint64_t id_;
    static std::atomic<uint64_t> next_id_;
    
public:
    // Constructors
    Thread();
    ~Thread();
    
    // Delete copy operations
    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;
    
    // Move operations
    Thread(Thread&& other) noexcept;
    Thread& operator=(Thread&& other) noexcept;
    
    // Spawn a new thread
    template<typename F, typename... Args>
    static Thread spawn(F&& func, Args&&... args) {
        Thread t;
        t.thread_ = std::make_unique<std::thread>(
            std::forward<F>(func), 
            std::forward<Args>(args)...
        );
        return t;
    }
    
    // Thread operations
    void join();
    void detach();
    bool joinable() const;
    
    // Properties
    uint64_t id() const { return id_; }
    const std::string& name() const { return name_; }
    void set_name(const std::string& name) { name_ = name; }
    
    // Static utilities
    static void sleep(uint64_t milliseconds);
    static void yield();
    static uint64_t current_id();
    static size_t hardware_concurrency();
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_RUNTIME_THREAD_H
