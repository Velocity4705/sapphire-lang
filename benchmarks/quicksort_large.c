#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
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

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    int n = 100000;
    int* arr = malloc(n * sizeof(int));
    
    // Fill with random data
    srand(42);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }
    
    double start = get_time();
    quicksort(arr, 0, n - 1);
    double end = get_time();
    
    printf("Sorted %d elements\n", n);
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    double elapsed = end - start;
    printf("Time: %.9f seconds\n", elapsed);
    printf("Time: %.3f milliseconds\n", elapsed * 1000.0);
    
    free(arr);
    return 0;
}
