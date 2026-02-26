// Matrix Multiplication
// Time Complexity: O(n^3)
// Space Complexity: O(n^2)

#include <stdio.h>
#include <stdlib.h>

#define SIZE 100

void matrix_multiply(long long A[SIZE][SIZE], long long B[SIZE][SIZE], long long C[SIZE][SIZE], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int n = 100;
    
    // Allocate matrices
    long long (*A)[SIZE] = malloc(sizeof(long long[SIZE][SIZE]));
    long long (*B)[SIZE] = malloc(sizeof(long long[SIZE][SIZE]));
    long long (*C)[SIZE] = malloc(sizeof(long long[SIZE][SIZE]));
    
    // Initialize matrices
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = i + j;
            B[i][j] = i * j;
        }
    }
    
    // Multiply
    matrix_multiply(A, B, C, n);
    
    // Sum for verification
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j; j++) {
            sum += C[i][j];
        }
    }
    
    printf("%lld\n", sum);
    
    free(A);
    free(B);
    free(C);
    
    return 0;
}
