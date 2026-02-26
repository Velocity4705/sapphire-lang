#include "mutex.h"

namespace sapphire {
namespace runtime {

void Mutex::lock() {
    mutex_.lock();
}

bool Mutex::try_lock() {
    return mutex_.try_lock();
}

void Mutex::unlock() {
    mutex_.unlock();
}

Mutex::Guard Mutex::lock_guard() {
    return Guard(*this);
}

// Guard implementation
Mutex::Guard::Guard(Mutex& m) : mutex_(m), locked_(true) {
    mutex_.lock();
}

Mutex::Guard::~Guard() {
    if (locked_) {
        mutex_.unlock();
    }
}

} // namespace runtime
} // namespace sapphire
