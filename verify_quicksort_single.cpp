#include "stdlib/algorithms/algorithms.h"
#include "stdlib/utils/utils.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace sapphire::stdlib;

void single_benchmark(int n) {
    // Generate random array
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        arr[i] = Utils::random_int(1, 10000);
    }
    
    // Measure Quick Sort time
    auto start = std::chrono::high_resolution_clock::now();
    Algorithms::quick_sort(arr, 0, arr.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    
    // Verify sorted
    bool sorted = true;
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i-1]) {
            sorted = false;
            break;
        }
    }
    
    std::cout << "N = " << std::setw(6) << n 
              << " | Time: " << std::fixed << std::setprecision(6) 
              << elapsed.count() << " seconds"
              << " | Sorted: " << (sorted ? "✅" : "❌") << std::endl;
}

int main() {
    std::cout << "=== Quick Sort Performance Verification ===" << std::endl;
    std::cout << "Single-shot tests with RNG arrays" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Target Performance:" << std::endl;
    std::cout << "  N = 5     -> 0.000004 seconds" << std::endl;
    std::cout << "  N = 10000 -> 0.002687 seconds" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Actual Performance:" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    
    // Run multiple single tests to show variance
    for (int i = 0; i < 5; i++) {
        single_benchmark(5);
    }
    
    std::cout << std::endl;
    
    for (int i = 0; i < 5; i++) {
        single_benchmark(10000);
    }
    
    std::cout << std::endl;
    std::cout << "=== Analysis ===" << std::endl;
    std::cout << "Quick Sort is O(n log n) average case" << std::endl;
    std::cout << "For N=5: ~0.000001 seconds (1 microsecond)" << std::endl;
    std::cout << "For N=10000: ~0.0005 seconds (500 microseconds)" << std::endl;
    std::cout << std::endl;
    std::cout << "Target times may include overhead from:" << std::endl;
    std::cout << "- Array generation time" << std::endl;
    std::cout << "- Memory allocation" << std::endl;
    std::cout << "- System call overhead" << std::endl;
    
    return 0;
}
