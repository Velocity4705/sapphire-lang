// LeetCode 1250: Check If It Is a Good Array
//
// Problem: Given an array of positive integers, determine if you can
// select a subset and multiply each by an integer to get a sum of 1.
//
// Solution: By Bézout's identity, this is possible if and only if
// the GCD of all numbers in the array is 1.
//
// Time Complexity: O(n * log(max(nums)))
// Space Complexity: O(1)

#include <stdio.h>
#include <stdbool.h>

// Euclidean algorithm for GCD
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Check if array is good
bool isGoodArray(int* nums, int numsSize) {
    if (numsSize == 0) return false;
    
    int result = nums[0];
    for (int i = 1; i < numsSize; i++) {
        result = gcd(result, nums[i]);
        // Early termination: if GCD becomes 1, we're done
        if (result == 1) {
            return true;
        }
    }
    
    return result == 1;
}

int main() {
    // Test Case 1: [12, 5, 7, 23]
    // Expected: true (1)
    int test1[] = {12, 5, 7, 23};
    printf("%d\n", isGoodArray(test1, 4));
    
    // Test Case 2: [29, 6, 10]
    // Expected: true (1)
    int test2[] = {29, 6, 10};
    printf("%d\n", isGoodArray(test2, 3));
    
    // Test Case 3: [3, 6]
    // Expected: false (0)
    int test3[] = {3, 6};
    printf("%d\n", isGoodArray(test3, 2));
    
    // Additional Test Cases
    
    // Test Case 4: [1] - single element 1
    // Expected: true (1)
    int test4[] = {1};
    printf("%d\n", isGoodArray(test4, 1));
    
    // Test Case 5: [2, 4, 6, 8] - all even
    // Expected: false (0)
    int test5[] = {2, 4, 6, 8};
    printf("%d\n", isGoodArray(test5, 4));
    
    // Test Case 6: [7, 14, 21] - multiples of 7
    // Expected: false (0)
    int test6[] = {7, 14, 21};
    printf("%d\n", isGoodArray(test6, 3));
    
    // Test Case 7: [10, 15, 21] - coprime pairs
    // Expected: true (1)
    int test7[] = {10, 15, 21};
    printf("%d\n", isGoodArray(test7, 3));
    
    return 0;
}
