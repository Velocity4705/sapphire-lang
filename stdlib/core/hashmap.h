#ifndef SAPPHIRE_STDLIB_HASHMAP_H
#define SAPPHIRE_STDLIB_HASHMAP_H

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

namespace sapphire {
namespace stdlib {

/**
 * HashMap<K, V> - Hash Map
 * 
 * Generic hash map with chaining for collision resolution.
 */
template<typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        
        Entry(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };
    
    Entry** buckets_;
    size_t size_;
    size_t capacity_;
    std::hash<K> hasher_;
    
    size_t hash(const K& key) const {
        return hasher_(key) % capacity_;
    }
    
    void rehash(size_t new_capacity) {
        Entry** old_buckets = buckets_;
        size_t old_capacity = capacity_;
        
        buckets_ = new Entry*[new_capacity]();
        capacity_ = new_capacity;
        size_ = 0;
        
        for (size_t i = 0; i < old_capacity; i++) {
            Entry* entry = old_buckets[i];
            while (entry) {
                insert(entry->key, entry->value);
                Entry* next = entry->next;
                delete entry;
                entry = next;
            }
        }
        
        delete[] old_buckets;
    }
    
public:
    // Constructors
    HashMap() : buckets_(nullptr), size_(0), capacity_(16) {
        buckets_ = new Entry*[capacity_]();
    }
    
    explicit HashMap(size_t initial_capacity) 
        : buckets_(nullptr), size_(0), capacity_(initial_capacity) {
        if (capacity_ < 8) capacity_ = 8;
        buckets_ = new Entry*[capacity_]();
    }
    
    HashMap(const HashMap& other) : buckets_(nullptr), size_(0), capacity_(other.capacity_) {
        buckets_ = new Entry*[capacity_]();
        for (size_t i = 0; i < other.capacity_; i++) {
            Entry* entry = other.buckets_[i];
            while (entry) {
                insert(entry->key, entry->value);
                entry = entry->next;
            }
        }
    }
    
    HashMap(HashMap&& other) noexcept
        : buckets_(other.buckets_), size_(other.size_), capacity_(other.capacity_) {
        other.buckets_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    ~HashMap() {
        clear();
        delete[] buckets_;
    }
    
    // Assignment
    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear();
            delete[] buckets_;
            capacity_ = other.capacity_;
            buckets_ = new Entry*[capacity_]();
            for (size_t i = 0; i < other.capacity_; i++) {
                Entry* entry = other.buckets_[i];
                while (entry) {
                    insert(entry->key, entry->value);
                    entry = entry->next;
                }
            }
        }
        return *this;
    }
    
    HashMap& operator=(HashMap&& other) noexcept {
        if (this != &other) {
            clear();
            delete[] buckets_;
            buckets_ = other.buckets_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.buckets_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }
    
    // Properties
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
    // Operations
    void insert(const K& key, const V& value) {
        if (size_ >= capacity_ * 0.75) {
            rehash(capacity_ * 2);
        }
        
        size_t index = hash(key);
        Entry* entry = buckets_[index];
        
        // Check if key exists
        while (entry) {
            if (entry->key == key) {
                entry->value = value;
                return;
            }
            entry = entry->next;
        }
        
        // Insert new entry
        Entry* new_entry = new Entry(key, value);
        new_entry->next = buckets_[index];
        buckets_[index] = new_entry;
        size_++;
    }
    
    V* get(const K& key) {
        size_t index = hash(key);
        Entry* entry = buckets_[index];
        
        while (entry) {
            if (entry->key == key) {
                return &entry->value;
            }
            entry = entry->next;
        }
        
        return nullptr;
    }
    
    const V* get(const K& key) const {
        size_t index = hash(key);
        Entry* entry = buckets_[index];
        
        while (entry) {
            if (entry->key == key) {
                return &entry->value;
            }
            entry = entry->next;
        }
        
        return nullptr;
    }
    
    V& operator[](const K& key) {
        V* value = get(key);
        if (value) return *value;
        
        // Insert default value
        insert(key, V());
        
        // Get the value again (pointer may have changed due to rehash)
        value = get(key);
        if (!value) {
            throw std::runtime_error("HashMap operator[] failed after insert");
        }
        return *value;
    }
    
    bool contains(const K& key) const {
        return get(key) != nullptr;
    }
    
    void remove(const K& key) {
        size_t index = hash(key);
        Entry* entry = buckets_[index];
        Entry* prev = nullptr;
        
        while (entry) {
            if (entry->key == key) {
                if (prev) {
                    prev->next = entry->next;
                } else {
                    buckets_[index] = entry->next;
                }
                delete entry;
                size_--;
                return;
            }
            prev = entry;
            entry = entry->next;
        }
    }
    
    void clear() {
        for (size_t i = 0; i < capacity_; i++) {
            Entry* entry = buckets_[i];
            while (entry) {
                Entry* next = entry->next;
                delete entry;
                entry = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }
    
    // Iteration support (simplified)
    class Iterator {
    private:
        HashMap* map_;
        size_t bucket_index_;
        Entry* entry_;
        
        void advance() {
            if (entry_ && entry_->next) {
                entry_ = entry_->next;
                return;
            }
            
            entry_ = nullptr;
            bucket_index_++;
            while (bucket_index_ < map_->capacity_ && !map_->buckets_[bucket_index_]) {
                bucket_index_++;
            }
            if (bucket_index_ < map_->capacity_) {
                entry_ = map_->buckets_[bucket_index_];
            }
        }
        
    public:
        Iterator(HashMap* map, size_t bucket, Entry* entry)
            : map_(map), bucket_index_(bucket), entry_(entry) {}
        
        std::pair<K, V> operator*() const {
            return {entry_->key, entry_->value};
        }
        
        Iterator& operator++() {
            advance();
            return *this;
        }
        
        bool operator!=(const Iterator& other) const {
            return entry_ != other.entry_;
        }
    };
    
    Iterator begin() {
        for (size_t i = 0; i < capacity_; i++) {
            if (buckets_[i]) {
                return Iterator(this, i, buckets_[i]);
            }
        }
        return end();
    }
    
    Iterator end() {
        return Iterator(this, capacity_, nullptr);
    }
};

} // namespace stdlib
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_HASHMAP_H
