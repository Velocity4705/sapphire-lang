#include <stdio.h>

int main() {
    long long a = 123456;
    long long b = 789012;

    long long c = a + b;
    long long d = a * b;
    long long e = a - b;
    long long f = a / 2;

    long long g = c + d;
    long long h = e + f;
    long long i = g * h;

    long long j = i + a;
    long long k = j + b;
    long long l = k + c;

    long long m = l * 2;
    long long n = m * 3;
    long long o = n * 4;

    long long p = o + a + b + c;
    long long q = p * 2;

    printf("%lld\n", q);
    return 0;
}
