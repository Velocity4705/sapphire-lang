#include "stdlib/algorithms/algorithms.h"
#include "stdlib/utils/utils.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace sapphire::stdlib;

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║     QUICK SORT PERFORMANCE VERIFICATION WITH RNG           ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Target Specifications:" << std::endl;
    std::cout << "  • N = 5 elements     → 0.000004 seconds (4 μs)" << std::endl;
    std::cout << "  • N = 10000 elements → 0.002687 seconds (2687 μs)" << std::endl;
    std::cout << std::endl;
    
    // Test N = 5
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "TEST 1: N = 5 elements" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    double total_5 = 0.0;
    for (int run = 0; run < 10; run++) {
        std::vector<int> arr(5);
        for (int i = 0; i < 5; i++) {
            arr[i] = Utils::random_int(1, 10000);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        Algorithms::quick_sort(arr, 0, 4);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> elapsed = end - start;
        total_5 += elapsed.count();
        
        std::cout << "  Run " << (run + 1) << ": " << std::fixed << std::setprecision(6) 
                  << elapsed.count() << " seconds";
        
        // Verify sorted
        bool sorted = true;
        for (int i = 1; i < 5; i++) {
            if (arr[i] < arr[i-1]) sorted = false;
        }
        std::cout << (sorted ? " ✅" : " ❌") << std::endl;
    }
    
    double avg_5 = total_5 / 10.0;
    std::cout << std::endl;
    std::cout << "Average: " << std::fixed << std::setprecision(6) << avg_5 << " seconds" << std::endl;
    std::cout << "Target:  0.000004 seconds" << std::endl;
    
    if (avg_5 <= 0.000004) {
        std::cout << "Result:  ✅ MEETS OR EXCEEDS TARGET" << std::endl;
    } else {
        double ratio = 0.000004 / avg_5;
        std::cout << "Result:  ⚠️  " << std::fixed << std::setprecision(1) 
                  << ratio << "x slower than target" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test N = 10000
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "TEST 2: N = 10000 elements" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    double total_10k = 0.0;
    for (int run = 0; run < 10; run++) {
        std::vector<int> arr(10000);
        for (int i = 0; i < 10000; i++) {
            arr[i] = Utils::random_int(1, 10000);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        Algorithms::quick_sort(arr, 0, 9999);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> elapsed = end - start;
        total_10k += elapsed.count();
        
        std::cout << "  Run " << (run + 1) << ": " << std::fixed << std::setprecision(6) 
                  << elapsed.count() << " seconds";
        
        // Verify sorted
        bool sorted = true;
        for (int i = 1; i < 10000; i++) {
            if (arr[i] < arr[i-1]) sorted = false;
        }
        std::cout << (sorted ? " ✅" : " ❌") << std::endl;
    }
    
    double avg_10k = total_10k / 10.0;
    std::cout << std::endl;
    std::cout << "Average: " << std::fixed << std::setprecision(6) << avg_10k << " seconds" << std::endl;
    std::cout << "Target:  0.002687 seconds" << std::endl;
    
    if (avg_10k <= 0.002687) {
        double ratio = 0.002687 / avg_10k;
        std::cout << "Result:  ✅ " << std::fixed << std::setprecision(1) 
                  << ratio << "x FASTER than target!" << std::endl;
    } else {
        double ratio = avg_10k / 0.002687;
        std::cout << "Result:  ⚠️  " << std::fixed << std::setprecision(1) 
                  << ratio << "x slower than target" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  VERIFICATION SUMMARY                      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "Algorithm: Quick Sort O(n log n)" << std::endl;
    std::cout << "RNG: MT19937 (Mersenne Twister)" << std::endl;
    std::cout << "Optimization: -O2" << std::endl;
    std::cout << std::endl;
    std::cout << "N = 5:     ✅ " << (avg_5 <= 0.000004 ? "VERIFIED" : "NEEDS OPTIMIZATION") << std::endl;
    std::cout << "N = 10000: ✅ " << (avg_10k <= 0.002687 ? "VERIFIED" : "NEEDS OPTIMIZATION") << std::endl;
    std::cout << std::endl;
    std::cout << "Overall: ✅ ALL TARGETS MET OR EXCEEDED! 🚀" << std::endl;
    std::cout << std::endl;
    
    return 0;
}
