#include "threadpool.h"

namespace sapphire {
namespace runtime {

ThreadPool::ThreadPool(size_t num_threads) : stop_(false), active_tasks_(0) {
    for (size_t i = 0; i < num_threads; i++) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
    }
    
    cv_.notify_all();
    
    for (std::thread& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            cv_.wait(lock, [this] { 
                return stop_ || !tasks_.empty(); 
            });
            
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            task = std::move(tasks_.front());
            tasks_.pop();
            active_tasks_++;
        }
        
        task();
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            active_tasks_--;
            if (tasks_.empty() && active_tasks_ == 0) {
                wait_cv_.notify_all();
            }
        }
    }
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(mutex_);
    wait_cv_.wait(lock, [this] { 
        return tasks_.empty() && active_tasks_ == 0; 
    });
}

size_t ThreadPool::queue_size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}

size_t ThreadPool::active_tasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return active_tasks_;
}

} // namespace runtime
} // namespace sapphire
