#include "rwlock.h"

namespace sapphire {
namespace runtime {

void RwLock::read_lock() {
    mutex_.lock_shared();
}

bool RwLock::try_read_lock() {
    return mutex_.try_lock_shared();
}

void RwLock::read_unlock() {
    mutex_.unlock_shared();
}

void RwLock::write_lock() {
    mutex_.lock();
}

bool RwLock::try_write_lock() {
    return mutex_.try_lock();
}

void RwLock::write_unlock() {
    mutex_.unlock();
}

RwLock::ReadGuard RwLock::read_guard() {
    return ReadGuard(*this);
}

RwLock::WriteGuard RwLock::write_guard() {
    return WriteGuard(*this);
}

// ReadGuard implementation
RwLock::ReadGuard::ReadGuard(RwLock& lock) : lock_(lock), locked_(true) {
    lock_.read_lock();
}

RwLock::ReadGuard::~ReadGuard() {
    if (locked_) {
        lock_.read_unlock();
    }
}

// WriteGuard implementation
RwLock::WriteGuard::WriteGuard(RwLock& lock) : lock_(lock), locked_(true) {
    lock_.write_lock();
}

RwLock::WriteGuard::~WriteGuard() {
    if (locked_) {
        lock_.write_unlock();
    }
}

} // namespace runtime
} // namespace sapphire
