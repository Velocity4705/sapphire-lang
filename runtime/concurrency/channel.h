#ifndef SAPPHIRE_RUNTIME_CHANNEL_H
#define SAPPHIRE_RUNTIME_CHANNEL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <stdexcept>

namespace sapphire {
namespace runtime {

/**
 * Channel<T> - Thread-safe message passing channel
 * 
 * Allows threads to communicate by sending and receiving messages.
 */
template<typename T>
class Channel {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;  // mutable so we can lock in const methods
    std::condition_variable cv_send_;
    std::condition_variable cv_recv_;
    size_t capacity_;
    bool closed_;
    
public:
    // Constructors
    Channel() : capacity_(0), closed_(false) {}
    
    explicit Channel(size_t capacity) : capacity_(capacity), closed_(false) {}
    
    ~Channel() {
        close();
    }
    
    // Delete copy operations
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    
    // Move operations
    Channel(Channel&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mutex_);
        queue_ = std::move(other.queue_);
        capacity_ = other.capacity_;
        closed_ = other.closed_;
        other.closed_ = true;
    }
    
    Channel& operator=(Channel&& other) noexcept {
        if (this != &other) {
            std::unique_lock<std::mutex> lock1(mutex_, std::defer_lock);
            std::unique_lock<std::mutex> lock2(other.mutex_, std::defer_lock);
            std::lock(lock1, lock2);
            
            queue_ = std::move(other.queue_);
            capacity_ = other.capacity_;
            closed_ = other.closed_;
            other.closed_ = true;
        }
        return *this;
    }
    
    // Send operations
    void send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (closed_) {
            throw std::runtime_error("Cannot send on closed channel");
        }
        
        // Wait if channel is full (buffered channel)
        if (capacity_ > 0) {
            cv_send_.wait(lock, [this] { 
                return queue_.size() < capacity_ || closed_; 
            });
            
            if (closed_) {
                throw std::runtime_error("Cannot send on closed channel");
            }
        }
        
        queue_.push(value);
        cv_recv_.notify_one();
    }
    
    void send(T&& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        if (closed_) {
            throw std::runtime_error("Cannot send on closed channel");
        }
        
        // Wait if channel is full (buffered channel)
        if (capacity_ > 0) {
            cv_send_.wait(lock, [this] { 
                return queue_.size() < capacity_ || closed_; 
            });
            
            if (closed_) {
                throw std::runtime_error("Cannot send on closed channel");
            }
        }
        
        queue_.push(std::move(value));
        cv_recv_.notify_one();
    }
    
    // Try send (non-blocking)
    bool try_send(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (closed_) {
            return false;
        }
        
        if (capacity_ > 0 && queue_.size() >= capacity_) {
            return false;
        }
        
        queue_.push(value);
        cv_recv_.notify_one();
        return true;
    }
    
    bool try_send(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (closed_) {
            return false;
        }
        
        if (capacity_ > 0 && queue_.size() >= capacity_) {
            return false;
        }
        
        queue_.push(std::move(value));
        cv_recv_.notify_one();
        return true;
    }
    
    // Receive operations
    T receive() {
        std::unique_lock<std::mutex> lock(mutex_);
        
        cv_recv_.wait(lock, [this] { 
            return !queue_.empty() || closed_; 
        });
        
        if (queue_.empty() && closed_) {
            throw std::runtime_error("Cannot receive from closed empty channel");
        }
        
        T value = std::move(queue_.front());
        queue_.pop();
        cv_send_.notify_one();
        return value;
    }
    
    // Try receive (non-blocking)
    std::optional<T> try_receive() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (queue_.empty()) {
            return std::nullopt;
        }
        
        T value = std::move(queue_.front());
        queue_.pop();
        cv_send_.notify_one();
        return value;
    }
    
    // Close channel
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        cv_send_.notify_all();
        cv_recv_.notify_all();
    }
    
    // Properties
    bool is_closed() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return closed_;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    size_t capacity() const {
        return capacity_;
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_RUNTIME_CHANNEL_H
