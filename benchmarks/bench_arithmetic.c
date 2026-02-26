// Arithmetic benchmark - C
// Tests: addition, multiplication, power operations

#include <stdio.h>
#include <math.h>

int main() {
    long long a = 123456;
    long long b = 789012;
    long long c = 0;

    // Test 1: Addition chain
    c = a + b;
    c = c + a;
    c = c + b;
    c = c + a;
    c = c + b;
    c = c + a;
    c = c + b;
    c = c + a;
    c = c + b;
    c = c + a;

    // Test 2: Multiplication chain
    c = a * b;
    c = c * 2;
    c = c * 3;
    c = c * 4;
    c = c * 5;

    // Test 3: Power operations
    c = (long long)pow(2, 10);
    c = (long long)pow(2, 15);
    c = (long long)pow(2, 20);

    // Test 4: Mixed operations
    c = (a + b) * (a - b);
    c = c + (a * b);
    c = c - (a / 2);

    printf("%lld\n", c);
    return 0;
}
