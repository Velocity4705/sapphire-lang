#include "stdlib/algorithms/algorithms.h"
#include "stdlib/utils/utils.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace sapphire::stdlib;

void benchmark_quicksort(int n, int runs = 100) {
    double total_time = 0.0;
    
    for (int run = 0; run < runs; run++) {
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
        total_time += elapsed.count();
        
        // Verify sorted
        for (size_t i = 1; i < arr.size(); i++) {
            if (arr[i] < arr[i-1]) {
                std::cerr << "ERROR: Array not sorted!" << std::endl;
                return;
            }
        }
    }
    
    double avg_time = total_time / runs;
    
    std::cout << "N = " << std::setw(6) << n 
              << " | Avg Time: " << std::fixed << std::setprecision(6) 
              << avg_time << " seconds"
              << " | Total: " << total_time << " seconds"
              << " | Runs: " << runs << std::endl;
}

int main() {
    std::cout << "=== Quick Sort Performance Verification ===" << std::endl;
    std::cout << "Using Random Number Generation (RNG)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Target Performance:" << std::endl;
    std::cout << "  N = 5     -> 0.000004 seconds" << std::endl;
    std::cout << "  N = 10000 -> 0.002687 seconds" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Actual Performance (100 runs each):" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    
    // Test with different sizes
    benchmark_quicksort(5, 100);
    benchmark_quicksort(10, 100);
    benchmark_quicksort(50, 100);
    benchmark_quicksort(100, 100);
    benchmark_quicksort(500, 100);
    benchmark_quicksort(1000, 100);
    benchmark_quicksort(5000, 100);
    benchmark_quicksort(10000, 100);
    
    std::cout << std::endl;
    std::cout << "=== Verification Complete ===" << std::endl;
    
    return 0;
}
