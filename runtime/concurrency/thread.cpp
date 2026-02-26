#include "thread.h"
#include <sstream>

namespace sapphire {
namespace runtime {

std::atomic<uint64_t> Thread::next_id_(1);

Thread::Thread() : thread_(nullptr), name_(""), id_(next_id_++) {}

Thread::Thread(Thread&& other) noexcept
    : thread_(std::move(other.thread_)),
      name_(std::move(other.name_)),
      id_(other.id_) {
    other.id_ = 0;
}

Thread& Thread::operator=(Thread&& other) noexcept {
    if (this != &other) {
        if (thread_ && thread_->joinable()) {
            thread_->detach();
        }
        thread_ = std::move(other.thread_);
        name_ = std::move(other.name_);
        id_ = other.id_;
        other.id_ = 0;
    }
    return *this;
}

Thread::~Thread() {
    if (thread_ && thread_->joinable()) {
        thread_->detach();
    }
}

void Thread::join() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void Thread::detach() {
    if (thread_ && thread_->joinable()) {
        thread_->detach();
    }
}

bool Thread::joinable() const {
    return thread_ && thread_->joinable();
}

void Thread::sleep(uint64_t milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Thread::yield() {
    std::this_thread::yield();
}

uint64_t Thread::current_id() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return std::stoull(ss.str());
}

size_t Thread::hardware_concurrency() {
    return std::thread::hardware_concurrency();
}

} // namespace runtime
} // namespace sapphire
