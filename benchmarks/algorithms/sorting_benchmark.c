// Sorting Algorithm Benchmark
// Quick Sort implementation
// Time Complexity: O(n log n) average
// Space Complexity: O(log n)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(long long* a, long long* b) {
    long long temp = *a;
    *a = *b;
    *b = temp;
}

int partition(long long arr[], int low, int high) {
    long long pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(long long arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

int main() {
    int n = 10000;
    long long* arr = malloc(n * sizeof(long long));
    
    // Initialize with random values
    srand(42);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }
    
    // Sort
    quicksort(arr, 0, n - 1);
    
    // Verify and sum
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    
    printf("%lld\n", sum);
    
    free(arr);
    return 0;
}
