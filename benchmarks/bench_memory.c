// Memory operations benchmark - C
// Tests: variable assignments and memory access

#include <stdio.h>

int main() {
    long long x1 = 100;
    long long x2 = 200;
    long long x3 = 300;
    long long x4 = 400;
    long long x5 = 500;

    // Multiple assignments
    long long y1 = x1 + x2;
    long long y2 = x2 + x3;
    long long y3 = x3 + x4;
    long long y4 = x4 + x5;
    long long y5 = x5 + x1;

    // Chained operations
    long long z1 = y1 + y2 + y3;
    long long z2 = y2 + y3 + y4;
    long long z3 = y3 + y4 + y5;

    // Final computation
    long long result = z1 + z2 + z3;

    printf("%lld\n", result);
    return 0;
}
