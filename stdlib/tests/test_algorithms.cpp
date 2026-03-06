#include "../algorithms/algorithms.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace sapphire::stdlib;

void test_sorting() {
    std::cout << "Testing sorting algorithms..." << std::endl;
    
    // Quick Sort
    std::vector<int> arr1 = {5, 2, 8, 1, 9, 3};
    Algorithms::quick_sort(arr1, 0, arr1.size() - 1);
    assert(arr1 == std::vector<int>({1, 2, 3, 5, 8, 9}));
    
    // Merge Sort
    std::vector<int> arr2 = {5, 2, 8, 1, 9, 3};
    Algorithms::merge_sort(arr2, 0, arr2.size() - 1);
    assert(arr2 == std::vector<int>({1, 2, 3, 5, 8, 9}));
    
    std::cout << "  ✓ Quick Sort and Merge Sort" << std::endl;
}

void test_searching() {
    std::cout << "Testing searching algorithms..." << std::endl;
    
    std::vector<int> arr = {1, 3, 5, 7, 9, 11, 13};
    
    // Binary Search
    assert(Algorithms::binary_search(arr, 7) == 3);
    assert(Algorithms::binary_search(arr, 1) == 0);
    assert(Algorithms::binary_search(arr, 13) == 6);
    assert(Algorithms::binary_search(arr, 4) == -1);
    
    // Linear Search
    assert(Algorithms::linear_search(arr, 7) == 3);
    assert(Algorithms::linear_search(arr, 99) == -1);
    
    std::cout << "  ✓ Binary Search and Linear Search" << std::endl;
}

void test_graph_algorithms() {
    std::cout << "Testing graph algorithms..." << std::endl;
    
    // BFS test
    std::vector<std::vector<int>> graph = {
        {1, 2},    // 0 -> 1, 2
        {0, 3},    // 1 -> 0, 3
        {0, 3},    // 2 -> 0, 3
        {1, 2}     // 3 -> 1, 2
    };
    
    auto bfs_result = Algorithms::bfs(graph, 0);
    assert(bfs_result.size() == 4);
    assert(bfs_result[0] == 0);
    
    // DFS test
    auto dfs_result = Algorithms::dfs(graph, 0);
    assert(dfs_result.size() == 4);
    assert(dfs_result[0] == 0);
    
    // Dijkstra test
    std::vector<std::vector<std::pair<int, int>>> weighted_graph = {
        {{1, 4}, {2, 1}},      // 0 -> 1(4), 2(1)
        {{3, 1}},              // 1 -> 3(1)
        {{1, 2}, {3, 5}},      // 2 -> 1(2), 3(5)
        {}                     // 3 -> none
    };
    
    auto dist = Algorithms::dijkstra(weighted_graph, 0);
    assert(dist[0] == 0);
    assert(dist[1] == 3);
    assert(dist[2] == 1);
    assert(dist[3] == 4);
    
    std::cout << "  ✓ BFS, DFS, and Dijkstra" << std::endl;
}

void test_dynamic_programming() {
    std::cout << "Testing dynamic programming..." << std::endl;
    
    // Fibonacci
    assert(Algorithms::fibonacci(0) == 0);
    assert(Algorithms::fibonacci(1) == 1);
    assert(Algorithms::fibonacci(10) == 55);
    assert(Algorithms::fibonacci(20) == 6765);
    
    // LCS
    assert(Algorithms::lcs_length("ABCDGH", "AEDFHR") == 3);
    assert(Algorithms::lcs_length("AGGTAB", "GXTXAYB") == 4);
    
    // Knapsack
    std::vector<int> weights = {2, 3, 4, 5};
    std::vector<int> values = {3, 4, 5, 6};
    assert(Algorithms::knapsack(weights, values, 5) == 7);
    
    std::cout << "  ✓ Fibonacci, LCS, and Knapsack" << std::endl;
}

void test_string_algorithms() {
    std::cout << "Testing string algorithms..." << std::endl;
    
    // KMP Search
    assert(Algorithms::kmp_search("ABABDABACDABABCABAB", "ABABCABAB") == 10);
    assert(Algorithms::kmp_search("hello world", "world") == 6);
    assert(Algorithms::kmp_search("hello world", "xyz") == -1);
    
    // Rabin-Karp
    auto matches = Algorithms::rabin_karp("AABAACAADAABAABA", "AABA");
    assert(matches.size() == 3);
    assert(matches[0] == 0);
    assert(matches[1] == 9);
    assert(matches[2] == 12);
    
    // Edit Distance
    assert(Algorithms::edit_distance("kitten", "sitting") == 3);
    assert(Algorithms::edit_distance("saturday", "sunday") == 3);
    
    std::cout << "  ✓ KMP, Rabin-Karp, and Edit Distance" << std::endl;
}

void test_c_api() {
    std::cout << "Testing C API..." << std::endl;
    
    int arr[] = {5, 2, 8, 1, 9};
    sapphire_quick_sort_int(arr, 5);
    assert(arr[0] == 1 && arr[4] == 9);
    
    int sorted[] = {1, 3, 5, 7, 9};
    assert(sapphire_binary_search_int(sorted, 5, 5) == 2);
    
    assert(sapphire_fibonacci(10) == 55);
    
    assert(sapphire_edit_distance("hello", "hallo") == 1);
    
    std::cout << "  ✓ C API functions" << std::endl;
}

int main() {
    std::cout << "=== Algorithms Library Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_sorting();
    test_searching();
    test_graph_algorithms();
    test_dynamic_programming();
    test_string_algorithms();
    test_c_api();
    
    std::cout << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    std::cout << "All Algorithms tests passed! 🧮✅" << std::endl;
    std::cout << "================================================================================";
    std::cout << std::endl;
    
    return 0;
}
