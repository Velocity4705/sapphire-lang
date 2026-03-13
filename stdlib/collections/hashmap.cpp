#include "hashmap.h"
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>

namespace sapphire {
namespace stdlib {
namespace Collections {

// HashMapEntry Implementation
HashMapEntry::HashMapEntry() : value(nullptr), value_size(0), is_occupied(false), hash(0) {}

HashMapEntry::~HashMapEntry() {
    clear();
}

void HashMapEntry::set_value(const void* val, size_t size) {
    // Clear existing value but preserve occupancy status
    if (value) {
        free(value);
        value = nullptr;
    }
    value_size = 0;
    
    if (val && size > 0) {
        value = malloc(size);
        if (value) {
            memcpy(value, val, size);
            value_size = size;
        }
    }
}

void* HashMapEntry::get_value() const {
    return value;
}

void HashMapEntry::clear() {
    if (value) {
        free(value);
        value = nullptr;
    }
    value_size = 0;
    is_occupied = false;
    key.clear();
    hash = 0;
}

// HashMap Implementation
HashMap::HashMap(size_t initial_capacity) 
    : buckets(nullptr), capacity(0), size(0), load_factor_threshold(75) {
    
    // Generate random seed for hash function
    std::random_device rd;
    seed = rd();
    
    // Ensure capacity is power of 2 and at least 16
    capacity = std::max(initial_capacity, size_t(16));
    size_t power = 1;
    while (power < capacity) power <<= 1;
    capacity = power;
    
    buckets = new HashMapEntry[capacity];
}

HashMap::~HashMap() {
    clear();
    delete[] buckets;
}

uint64_t HashMap::hash_key(const std::string& key) const {
    // FNV-1a hash with random seed
    uint64_t hash = 14695981039346656037ULL ^ seed;
    for (char c : key) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

size_t HashMap::find_slot(const std::string& key, uint64_t hash) const {
    size_t index = hash & (capacity - 1);  // Fast modulo for power of 2
    size_t original_index = index;
    
    // Linear probing
    while (buckets[index].is_occupied) {
        if (buckets[index].hash == hash && buckets[index].key == key) {
            return index;  // Found existing key
        }
        index = (index + 1) & (capacity - 1);
        if (index == original_index) {
            break;  // Table is full
        }
    }
    
    // Return the first empty slot we found, or the original index if table is full
    return index;
}

bool HashMap::set(const std::string& key, const void* value, size_t value_size) {
    // Resize if load factor too high
    if (size * 100 >= capacity * load_factor_threshold) {
        resize();
    }
    
    uint64_t hash = hash_key(key);
    size_t index = hash & (capacity - 1);
    size_t original_index = index;
    
    // Linear probing to find insertion point
    while (buckets[index].is_occupied) {
        if (buckets[index].hash == hash && buckets[index].key == key) {
            // Key exists, just update value
            buckets[index].set_value(value, value_size);
            return true;
        }
        index = (index + 1) & (capacity - 1);
        if (index == original_index) {
            // Table is full (shouldn't happen due to load factor check)
            return false;
        }
    }
    
    // Found empty slot, insert new entry
    buckets[index].key = key;
    buckets[index].hash = hash;
    buckets[index].is_occupied = true;
    buckets[index].set_value(value, value_size);
    size++;
    
    return true;
}

void* HashMap::get(const std::string& key) {
    uint64_t hash = hash_key(key);
    size_t index = hash & (capacity - 1);
    size_t original_index = index;
    
    // Linear probing to find the key
    while (buckets[index].is_occupied) {
        if (buckets[index].hash == hash && buckets[index].key == key) {
            return buckets[index].get_value();
        }
        index = (index + 1) & (capacity - 1);
        if (index == original_index) {
            break;  // Searched entire table
        }
    }
    
    return nullptr;
}

bool HashMap::remove(const std::string& key) {
    uint64_t hash = hash_key(key);
    size_t index = find_slot(key, hash);
    
    if (buckets[index].is_occupied && buckets[index].key == key) {
        buckets[index].clear();
        size--;
        
        // Rehash entries that might have been displaced
        size_t next_index = (index + 1) & (capacity - 1);
        while (buckets[next_index].is_occupied) {
            HashMapEntry temp = std::move(buckets[next_index]);
            buckets[next_index].clear();
            size--;
            
            // Re-insert the displaced entry
            set(temp.key, temp.get_value(), temp.value_size);
            
            next_index = (next_index + 1) & (capacity - 1);
        }
        
        return true;
    }
    
    return false;
}

bool HashMap::contains(const std::string& key) {
    return get(key) != nullptr;
}

void HashMap::clear() {
    for (size_t i = 0; i < capacity; i++) {
        buckets[i].clear();
    }
    size = 0;
}

bool HashMap::empty() const {
    return size == 0;
}

double HashMap::load_factor() const {
    return capacity > 0 ? static_cast<double>(size) / capacity : 0.0;
}

void HashMap::resize() {
    rehash(capacity * 2);
}

void HashMap::rehash(size_t new_capacity) {
    // Save old data
    HashMapEntry* old_buckets = buckets;
    size_t old_capacity = capacity;
    
    // Create new table
    capacity = new_capacity;
    buckets = new HashMapEntry[capacity];
    size = 0;
    
    // Re-insert all entries
    for (size_t i = 0; i < old_capacity; i++) {
        if (old_buckets[i].is_occupied) {
            set(old_buckets[i].key, old_buckets[i].get_value(), old_buckets[i].value_size);
        }
    }
    
    // Clean up old table
    delete[] old_buckets;
}

std::vector<std::string> HashMap::keys() const {
    std::vector<std::string> result;
    result.reserve(size);
    
    for (size_t i = 0; i < capacity; i++) {
        if (buckets[i].is_occupied) {
            result.push_back(buckets[i].key);
        }
    }
    
    return result;
}

// Factory functions
HashMap* create_hashmap(size_t initial_capacity) {
    return new HashMap(initial_capacity);
}

void destroy_hashmap(HashMap* map) {
    delete map;
}

// HashMap operations
bool hashmap_set(HashMap* map, const std::string& key, const void* value, size_t value_size) {
    return map ? map->set(key, value, value_size) : false;
}

void* hashmap_get(HashMap* map, const std::string& key) {
    return map ? map->get(key) : nullptr;
}

bool hashmap_remove(HashMap* map, const std::string& key) {
    return map ? map->remove(key) : false;
}

bool hashmap_contains(HashMap* map, const std::string& key) {
    return map ? map->contains(key) : false;
}

size_t hashmap_size(HashMap* map) {
    return map ? map->size : 0;
}

bool hashmap_empty(HashMap* map) {
    return map ? map->empty() : true;
}

void hashmap_clear(HashMap* map) {
    if (map) {
        map->clear();
    }
}

// Specialized functions for common types
bool hashmap_set_int(HashMap* map, const std::string& key, int64_t value) {
    return map ? map->set(key, &value, sizeof(int64_t)) : false;
}

int64_t hashmap_get_int(HashMap* map, const std::string& key) {
    if (map) {
        void* ptr = map->get(key);
        if (ptr) {
            return *(int64_t*)ptr;
        }
    }
    return 0;
}

bool hashmap_set_double(HashMap* map, const std::string& key, double value) {
    return map ? map->set(key, &value, sizeof(double)) : false;
}

double hashmap_get_double(HashMap* map, const std::string& key) {
    if (map) {
        void* ptr = map->get(key);
        if (ptr) {
            return *(double*)ptr;
        }
    }
    return 0.0;
}

bool hashmap_set_string(HashMap* map, const std::string& key, const std::string& value) {
    if (map) {
        // Store string as C string
        return map->set(key, value.c_str(), value.length() + 1);
    }
    return false;
}

std::string hashmap_get_string(HashMap* map, const std::string& key) {
    if (map) {
        void* ptr = map->get(key);
        if (ptr) {
            return std::string((char*)ptr);
        }
    }
    return "";
}

} // namespace Collections
} // namespace stdlib
} // namespace sapphire

// C API Implementation
extern "C" {
    static thread_local char g_hashmap_string_buffer[8192];
    
    void* sapphire_hashmap_create() {
        return sapphire::stdlib::Collections::create_hashmap();
    }
    
    void* sapphire_hashmap_create_with_capacity(int64_t capacity) {
        return sapphire::stdlib::Collections::create_hashmap(capacity > 0 ? capacity : 16);
    }
    
    void sapphire_hashmap_destroy(void* map) {
        if (map) {
            sapphire::stdlib::Collections::destroy_hashmap(
                (sapphire::stdlib::Collections::HashMap*)map);
        }
    }
    
    bool sapphire_hashmap_set_int(void* map, const char* key, int64_t value) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_set_int(
                (sapphire::stdlib::Collections::HashMap*)map, key, value);
        }
        return false;
    }
    
    int64_t sapphire_hashmap_get_int(void* map, const char* key) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_get_int(
                (sapphire::stdlib::Collections::HashMap*)map, key);
        }
        return 0;
    }
    
    bool sapphire_hashmap_set_double(void* map, const char* key, double value) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_set_double(
                (sapphire::stdlib::Collections::HashMap*)map, key, value);
        }
        return false;
    }
    
    double sapphire_hashmap_get_double(void* map, const char* key) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_get_double(
                (sapphire::stdlib::Collections::HashMap*)map, key);
        }
        return 0.0;
    }
    
    bool sapphire_hashmap_set_string(void* map, const char* key, const char* value) {
        if (map && key && value) {
            return sapphire::stdlib::Collections::hashmap_set_string(
                (sapphire::stdlib::Collections::HashMap*)map, key, value);
        }
        return false;
    }
    
    const char* sapphire_hashmap_get_string(void* map, const char* key) {
        if (map && key) {
            std::string result = sapphire::stdlib::Collections::hashmap_get_string(
                (sapphire::stdlib::Collections::HashMap*)map, key);
            
            strncpy(g_hashmap_string_buffer, result.c_str(), sizeof(g_hashmap_string_buffer) - 1);
            g_hashmap_string_buffer[sizeof(g_hashmap_string_buffer) - 1] = '\0';
            
            return g_hashmap_string_buffer;
        }
        return nullptr;
    }
    
    bool sapphire_hashmap_remove(void* map, const char* key) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_remove(
                (sapphire::stdlib::Collections::HashMap*)map, key);
        }
        return false;
    }
    
    bool sapphire_hashmap_contains(void* map, const char* key) {
        if (map && key) {
            return sapphire::stdlib::Collections::hashmap_contains(
                (sapphire::stdlib::Collections::HashMap*)map, key);
        }
        return false;
    }
    
    int64_t sapphire_hashmap_size(void* map) {
        if (map) {
            return (int64_t)sapphire::stdlib::Collections::hashmap_size(
                (sapphire::stdlib::Collections::HashMap*)map);
        }
        return 0;
    }
    
    bool sapphire_hashmap_empty(void* map) {
        if (map) {
            return sapphire::stdlib::Collections::hashmap_empty(
                (sapphire::stdlib::Collections::HashMap*)map);
        }
        return true;
    }
    
    void sapphire_hashmap_clear(void* map) {
        if (map) {
            sapphire::stdlib::Collections::hashmap_clear(
                (sapphire::stdlib::Collections::HashMap*)map);
        }
    }
}