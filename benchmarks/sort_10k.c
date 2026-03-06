#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main() {
    int n = 10000;
    int* arr = malloc(n * sizeof(int));
    
    // Fill with reverse sorted data (worst case)
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    
    double start = get_time();
    qsort(arr, n, sizeof(int), compare);
    double end = get_time();
    
    printf("Sorted %d elements\n", n);
    printf("First 10: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    double elapsed = end - start;
    printf("Time: %.9f seconds\n", elapsed);
    printf("Time: %.6f milliseconds\n", elapsed * 1000.0);
    
    if (elapsed < 0.003) {
        printf("✓ VERIFIED: Under 0.003 seconds\n");
    }
    
    free(arr);
    return 0;
}
