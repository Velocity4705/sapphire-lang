#ifndef SAPPHIRE_RUNTIME_THREADPOOL_H
#define SAPPHIRE_RUNTIME_THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>

namespace sapphire {
namespace runtime {

/**
 * ThreadPool - Pool of worker threads for parallel task execution
 * 
 * Manages a fixed number of worker threads that execute submitted tasks.
 */
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;  // mutable for const methods
    std::condition_variable cv_;
    bool stop_;
    size_t active_tasks_;
    std::condition_variable wait_cv_;
    
    void worker_thread();
    
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    // Delete copy and move operations
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    
    // Submit a task
    template<typename F, typename... Args>
    auto submit(F&& func, Args&&... args) 
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            if (stop_) {
                throw std::runtime_error("Cannot submit task to stopped ThreadPool");
            }
            
            tasks_.emplace([task]() { (*task)(); });
        }
        
        cv_.notify_one();
        return result;
    }
    
    // Wait for all tasks to complete
    void wait_all();
    
    // Properties
    size_t num_threads() const { return workers_.size(); }
    size_t queue_size() const;
    size_t active_tasks() const;
};

} // namespace runtime
} // namespace sapphire

#endif // SAPPHIRE_RUNTIME_THREADPOOL_H
