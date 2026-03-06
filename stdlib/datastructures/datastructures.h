#ifndef SAPPHIRE_STDLIB_DATASTRUCTURES_H
#define SAPPHIRE_STDLIB_DATASTRUCTURES_H

#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace sapphire {
namespace stdlib {

/**
 * Data Structures - High-Performance Data Structures
 * 
 * All operations annotated with time and space complexity.
 */

// Stack - LIFO - O(1) push/pop/top, O(n) space
template<typename T>
class Stack {
private:
    std::vector<T> data_;
    
public:
    void push(const T& value) { data_.push_back(value); }  // O(1) amortized
    T pop() {  // O(1)
        T val = data_.back();
        data_.pop_back();
        return val;
    }
    T top() const { return data_.back(); }  // O(1)
    bool empty() const { return data_.empty(); }  // O(1)
    size_t size() const { return data_.size(); }  // O(1)
};

// Queue - FIFO - O(1) enqueue/dequeue, O(n) space
template<typename T>
class Queue {
private:
    std::queue<T> data_;
    
public:
    void enqueue(const T& value) { data_.push(value); }  // O(1)
    T dequeue() {  // O(1)
        T val = data_.front();
        data_.pop();
        return val;
    }
    T front() const { return data_.front(); }  // O(1)
    bool empty() const { return data_.empty(); }  // O(1)
    size_t size() const { return data_.size(); }  // O(1)
};

// Priority Queue - O(log n) insert/extract, O(n) space
template<typename T>
class PriorityQueue {
private:
    std::priority_queue<T> data_;
    
public:
    void insert(const T& value) { data_.push(value); }  // O(log n)
    T extract_max() {  // O(log n)
        T val = data_.top();
        data_.pop();
        return val;
    }
    T peek() const { return data_.top(); }  // O(1)
    bool empty() const { return data_.empty(); }  // O(1)
    size_t size() const { return data_.size(); }  // O(1)
};

// HashMap - O(1) average insert/lookup/delete, O(n) space
template<typename K, typename V>
class HashMap {
private:
    std::unordered_map<K, V> data_;
    
public:
    void insert(const K& key, const V& value) { data_[key] = value; }  // O(1) average
    V get(const K& key) const {  // O(1) average
        auto it = data_.find(key);
        return (it != data_.end()) ? it->second : V{};
    }
    bool contains(const K& key) const { return data_.find(key) != data_.end(); }  // O(1) average
    void remove(const K& key) { data_.erase(key); }  // O(1) average
    size_t size() const { return data_.size(); }  // O(1)
    bool empty() const { return data_.empty(); }  // O(1)
};

// HashSet - O(1) average insert/lookup/delete, O(n) space
template<typename T>
class HashSet {
private:
    std::unordered_set<T> data_;
    
public:
    void insert(const T& value) { data_.insert(value); }  // O(1) average
    bool contains(const T& value) const { return data_.find(value) != data_.end(); }  // O(1) average
    void remove(const T& value) { data_.erase(value); }  // O(1) average
    size_t size() const { return data_.size(); }  // O(1)
    bool empty() const { return data_.empty(); }  // O(1)
};

// Binary Search Tree Node
template<typename T>
struct BSTNode {
    T value;
    std::unique_ptr<BSTNode<T>> left;
    std::unique_ptr<BSTNode<T>> right;
    
    explicit BSTNode(const T& val) : value(val), left(nullptr), right(nullptr) {}
};

// Binary Search Tree - O(log n) average, O(n) worst for insert/search/delete
template<typename T>
class BST {
private:
    std::unique_ptr<BSTNode<T>> root_;
    size_t size_;
    
    BSTNode<T>* insert_helper(std::unique_ptr<BSTNode<T>>& node, const T& value) {
        if (!node) {
            node = std::make_unique<BSTNode<T>>(value);
            size_++;
            return node.get();
        }
        
        if (value < node->value) {
            return insert_helper(node->left, value);
        } else if (value > node->value) {
            return insert_helper(node->right, value);
        }
        return node.get();
    }
    
    bool search_helper(const BSTNode<T>* node, const T& value) const {
        if (!node) return false;
        if (node->value == value) return true;
        if (value < node->value) return search_helper(node->left.get(), value);
        return search_helper(node->right.get(), value);
    }
    
public:
    BST() : root_(nullptr), size_(0) {}
    
    void insert(const T& value) { insert_helper(root_, value); }  // O(log n) average
    bool search(const T& value) const { return search_helper(root_.get(), value); }  // O(log n) average
    size_t size() const { return size_; }  // O(1)
    bool empty() const { return size_ == 0; }  // O(1)
};

// Trie - O(m) insert/search where m is key length, O(ALPHABET_SIZE * N * M) space
class Trie {
private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool is_end_of_word;
        
        TrieNode() : is_end_of_word(false) {}
    };
    
    std::unique_ptr<TrieNode> root_;
    
public:
    Trie() : root_(std::make_unique<TrieNode>()) {}
    
    void insert(const std::string& word) {  // O(m)
        TrieNode* node = root_.get();
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = std::make_unique<TrieNode>();
            }
            node = node->children[c].get();
        }
        node->is_end_of_word = true;
    }
    
    bool search(const std::string& word) const {  // O(m)
        const TrieNode* node = root_.get();
        for (char c : word) {
            auto it = node->children.find(c);
            if (it == node->children.end()) return false;
            node = it->second.get();
        }
        return node->is_end_of_word;
    }
    
    bool starts_with(const std::string& prefix) const {  // O(m)
        const TrieNode* node = root_.get();
        for (char c : prefix) {
            auto it = node->children.find(c);
            if (it == node->children.end()) return false;
            node = it->second.get();
        }
        return true;
    }
};

} // namespace stdlib
} // namespace sapphire

#endif // SAPPHIRE_STDLIB_DATASTRUCTURES_H
