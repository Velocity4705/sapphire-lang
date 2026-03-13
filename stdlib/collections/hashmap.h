#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <functional>

namespace sapphire {
namespace stdlib {
namespace Collections {

// Hash Map Entry
struct HashMapEntry {
    std::string key;
    void* value;
    size_t value_size;
    bool is_occupied;
    uint64_t hash;
    
    HashMapEntry();
    ~HashMapEntry();
    
    void set_value(const void* val, size_t size);
    void* get_value() const;
    void clear();
};

// Hash Map Structure
struct HashMap {
    HashMapEntry* buckets;
    size_t capacity;
    size_t size;
    size_t load_factor_threshold;  // Resize when size > capacity * threshold / 100
    uint64_t seed;  // For hash randomization
    
    HashMap(size_t initial_capacity = 16);
    ~HashMap();
    
    // Core operations
    bool set(const std::string& key, const void* value, size_t value_size);
    void* get(const std::string& key);
    bool remove(const std::string& key);
    bool contains(const std::string& key);
    void clear();
    
    // Utility
    bool empty() const;
    double load_factor() const;
    void reserve(size_t new_capacity);
    
    // Iterator support
    std::vector<std::string> keys() const;
    
private:
    uint64_t hash_key(const std::string& key) const;
    size_t find_slot(const std::string& key, uint64_t hash) const;
    void resize();
    void rehash(size_t new_capacity);
};

// Factory functions
HashMap* create_hashmap(size_t initial_capacity = 16);
void destroy_hashmap(HashMap* map);

// HashMap operations
bool hashmap_set(HashMap* map, const std::string& key, const void* value, size_t value_size);
void* hashmap_get(HashMap* map, const std::string& key);
bool hashmap_remove(HashMap* map, const std::string& key);
bool hashmap_contains(HashMap* map, const std::string& key);
size_t hashmap_size(HashMap* map);
bool hashmap_empty(HashMap* map);
void hashmap_clear(HashMap* map);

// Specialized functions for common types
bool hashmap_set_int(HashMap* map, const std::string& key, int64_t value);
int64_t hashmap_get_int(HashMap* map, const std::string& key);

bool hashmap_set_double(HashMap* map, const std::string& key, double value);
double hashmap_get_double(HashMap* map, const std::string& key);

bool hashmap_set_string(HashMap* map, const std::string& key, const std::string& value);
std::string hashmap_get_string(HashMap* map, const std::string& key);

} // namespace Collections
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // HashMap lifecycle
    void* sapphire_hashmap_create();
    void* sapphire_hashmap_create_with_capacity(int64_t capacity);
    void sapphire_hashmap_destroy(void* map);
    
    // Core operations
    bool sapphire_hashmap_set_int(void* map, const char* key, int64_t value);
    int64_t sapphire_hashmap_get_int(void* map, const char* key);
    
    bool sapphire_hashmap_set_double(void* map, const char* key, double value);
    double sapphire_hashmap_get_double(void* map, const char* key);
    
    bool sapphire_hashmap_set_string(void* map, const char* key, const char* value);
    const char* sapphire_hashmap_get_string(void* map, const char* key);
    
    bool sapphire_hashmap_remove(void* map, const char* key);
    bool sapphire_hashmap_contains(void* map, const char* key);
    
    // Utility functions
    int64_t sapphire_hashmap_size(void* map);
    bool sapphire_hashmap_empty(void* map);
    void sapphire_hashmap_clear(void* map);
}