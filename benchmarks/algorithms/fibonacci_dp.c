// Dynamic Programming: Fibonacci
// Time Complexity: O(n)
// Space Complexity: O(n)

#include <stdio.h>
#include <stdlib.h>

long long fibonacci_dp(int n) {
    if (n <= 1) return n;
    
    long long *dp = (long long*)malloc((n + 1) * sizeof(long long));
    dp[0] = 0;
    dp[1] = 1;
    
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i-1] + dp[i-2];
    }
    
    long long result = dp[n];
    free(dp);
    return result;
}

int main() {
    int n = 40;
    long long result = fibonacci_dp(n);
    printf("%lld\n", result);
    return 0;
}
