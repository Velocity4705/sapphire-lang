#ifndef SAPPHIRE_STDLIB_VEC_H
#define SAPPHIRE_STDLIB_VEC_H

#include <cstddef>
#include <stdexcept>
#include <algorithm>

namespace sapphire {
namespace stdlib {

/**
 * Vec<T> - Dynamic Array
 * 
 * Generic dynamic array with automatic resizing.
 */
template<typename T>
class Vec {
private:
    T* data_;
    size_t size_;
    size_t capacity_;
    
    void ensure_capacity(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        
        size_t new_cap = capacity_ == 0 ? 8 : capacity_;
        while (new_cap < new_capacity) {
            new_cap *= 2;
        }
        
        T* new_data = new T[new_cap];
        for (size_t i = 0; i < size_; i++) {
            new_data[i] = std::move(data_[i]);
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
    }
    
public:
    // Constructors
    Vec() : data_(nullptr), size_(0), capacity_(0) {}
    
    explicit Vec(size_t initial_capacity) : data_(nullptr), size_(0), capacity_(0) {
        reserve(initial_capacity);
    }
    
    Vec(const Vec& other) : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.size_);
        for (size_t i = 0; i < other.size_; i++) {
            push(other.data_[i]);
        }
    }
    
    Vec(Vec&& other) noexcept 
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    ~Vec() {
        delete[] data_;
    }
    
    // Assignment
    Vec& operator=(const Vec& other) {
        if (this != &other) {
            clear();
            reserve(other.size_);
            for (size_t i = 0; i < other.size_; i++) {
                push(other.data_[i]);
            }
        }
        return *this;
    }
    
    Vec& operator=(Vec&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    // Properties
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    
    // Access
    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    
    T& at(size_t index) {
        if (index >= size_) throw std::out_of_range("Vec index out of range");
        return data_[index];
    }
    
    const T& at(size_t index) const {
        if (index >= size_) throw std::out_of_range("Vec index out of range");
        return data_[index];
    }
    
    T& front() { return data_[0]; }
    const T& front() const { return data_[0]; }
    T& back() { return data_[size_ - 1]; }
    const T& back() const { return data_[size_ - 1]; }
    
    // Modification
    void push(const T& value) {
        ensure_capacity(size_ + 1);
        data_[size_++] = value;
    }
    
    void push(T&& value) {
        ensure_capacity(size_ + 1);
        data_[size_++] = std::move(value);
    }
    
    T pop() {
        if (empty()) throw std::out_of_range("Vec is empty");
        return std::move(data_[--size_]);
    }
    
    void insert(size_t index, const T& value) {
        if (index > size_) throw std::out_of_range("Vec index out of range");
        ensure_capacity(size_ + 1);
        for (size_t i = size_; i > index; i--) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = value;
        size_++;
    }
    
    void remove(size_t index) {
        if (index >= size_) throw std::out_of_range("Vec index out of range");
        for (size_t i = index; i < size_ - 1; i++) {
            data_[i] = std::move(data_[i + 1]);
        }
        size_--;
    }
    
    void clear() {
        size_ = 0;
    }
    
    void reserve(size_t capacity) {
        ensure_capacity(capacity);
    }
    
    void resize(size_t new_size) {
        if (new_size > size_) {
            ensure_capacity(new_size);
            for (size_t i = size_; i < new_size; i++) {
                data_[i] = T();
            }
        }
        size_ = new_size;
    }
    
    // Iteration
    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }
    
    // Data access
    T* data() { return data_; }
    const T* data() const { return data_; }
};

} // namespace stdlib
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_VEC_H
