// Hash Operations Benchmark
// Time Complexity: O(1) average for insert/lookup
// Space Complexity: O(n)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10000

typedef struct Entry {
    long long key;
    long long value;
    struct Entry* next;
} Entry;

typedef struct {
    Entry* buckets[TABLE_SIZE];
} HashMap;

unsigned long hash(long long key) {
    return key % TABLE_SIZE;
}

void insert(HashMap* map, long long key, long long value) {
    unsigned long index = hash(key);
    Entry* entry = malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
}

long long get(HashMap* map, long long key) {
    unsigned long index = hash(key);
    Entry* entry = map->buckets[index];
    while (entry != NULL) {
        if (entry->key == key) {
            return entry->value;
        }
        entry = entry->next;
    }
    return -1;
}

int main() {
    HashMap map = {0};
    
    // Insert 1000 key-value pairs
    for (long long i = 0; i < 1000; i++) {
        insert(&map, i, i * 2);
    }
    
    // Lookup and sum
    long long sum = 0;
    for (long long i = 0; i < 1000; i++) {
        sum += get(&map, i);
    }
    
    printf("%lld\n", sum);
    
    // Cleanup
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry* entry = map.buckets[i];
        while (entry != NULL) {
            Entry* next = entry->next;
            free(entry);
            entry = next;
        }
    }
    
    return 0;
}
