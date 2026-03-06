#ifndef SAPPHIRE_STDLIB_ALGORITHMS_H
#define SAPPHIRE_STDLIB_ALGORITHMS_H

#include <vector>
#include <functional>
#include <algorithm>
#include <string>
#include <cstddef>

namespace sapphire {
namespace stdlib {

/**
 * Algorithms - High-Performance Algorithms with Complexity Guarantees
 * 
 * All algorithms are annotated with time and space complexity.
 */

namespace Algorithms {
    // Sorting Algorithms
    
    // Quick Sort - O(n log n) average, O(n²) worst, O(log n) space
    template<typename T>
    void quick_sort(std::vector<T>& arr, int left, int right) {
        if (left >= right) return;
        
        T pivot = arr[left + (right - left) / 2];
        int i = left, j = right;
        
        while (i <= j) {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) {
                std::swap(arr[i], arr[j]);
                i++;
                j--;
            }
        }
        
        if (left < j) quick_sort(arr, left, j);
        if (i < right) quick_sort(arr, i, right);
    }
    
    // Merge Sort - O(n log n) time, O(n) space
    template<typename T>
    void merge_sort(std::vector<T>& arr, int left, int right) {
        if (left >= right) return;
        
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        
        std::vector<T> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;
        
        while (i <= mid && j <= right) {
            if (arr[i] <= arr[j]) temp[k++] = arr[i++];
            else temp[k++] = arr[j++];
        }
        while (i <= mid) temp[k++] = arr[i++];
        while (j <= right) temp[k++] = arr[j++];
        
        for (int i = 0; i < k; i++) {
            arr[left + i] = temp[i];
        }
    }
    
    // Heap Sort - O(n log n) time, O(1) space
    template<typename T>
    void heap_sort(std::vector<T>& arr);
    
    // Searching Algorithms
    
    // Binary Search - O(log n) time, O(1) space
    template<typename T>
    int binary_search(const std::vector<T>& arr, const T& target) {
        int left = 0, right = arr.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] == target) return mid;
            if (arr[mid] < target) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
    
    // Linear Search - O(n) time, O(1) space
    template<typename T>
    int linear_search(const std::vector<T>& arr, const T& target) {
        for (size_t i = 0; i < arr.size(); i++) {
            if (arr[i] == target) return i;
        }
        return -1;
    }
    
    // Graph Algorithms
    
    // Dijkstra's Shortest Path - O((V + E) log V) time, O(V) space
    std::vector<int> dijkstra(const std::vector<std::vector<std::pair<int, int>>>& graph, int start);
    
    // BFS - O(V + E) time, O(V) space
    std::vector<int> bfs(const std::vector<std::vector<int>>& graph, int start);
    
    // DFS - O(V + E) time, O(V) space
    std::vector<int> dfs(const std::vector<std::vector<int>>& graph, int start);
    
    // Dynamic Programming
    
    // Fibonacci - O(n) time, O(1) space
    int64_t fibonacci(int n);
    
    // Longest Common Subsequence - O(m*n) time, O(m*n) space
    int lcs_length(const std::string& s1, const std::string& s2);
    
    // Knapsack - O(n*W) time, O(n*W) space
    int knapsack(const std::vector<int>& weights, const std::vector<int>& values, int capacity);
    
    // String Algorithms
    
    // KMP Pattern Matching - O(n + m) time, O(m) space
    int kmp_search(const std::string& text, const std::string& pattern);
    
    // Rabin-Karp - O(n + m) average, O(nm) worst
    std::vector<int> rabin_karp(const std::string& text, const std::string& pattern);
    
    // Levenshtein Distance - O(m*n) time, O(m*n) space
    int edit_distance(const std::string& s1, const std::string& s2);
}

} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    void sapphire_quick_sort_int(int* arr, int size);
    void sapphire_merge_sort_int(int* arr, int size);
    int sapphire_binary_search_int(const int* arr, int size, int target);
    int64_t sapphire_fibonacci(int n);
    int sapphire_edit_distance(const char* s1, const char* s2);
}

#endif // SAPPHIRE_STDLIB_ALGORITHMS_H
