// String Manipulation Benchmark
// Time Complexity: O(n) for most operations
// Space Complexity: O(n)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// String concatenation
char* string_concat(const char* s1, const char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char* result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// String reversal
void string_reverse(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

// Pattern matching (naive)
int pattern_match(const char* text, const char* pattern) {
    int n = strlen(text);
    int m = strlen(pattern);
    int count = 0;
    
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        if (j == m) {
            count++;
        }
    }
    
    return count;
}

int main() {
    // Create test strings
    char str1[1000];
    char str2[1000];
    
    // Fill with pattern
    for (int i = 0; i < 999; i++) {
        str1[i] = 'a' + (i % 26);
        str2[i] = 'a' + ((i + 5) % 26);
    }
    str1[999] = '\0';
    str2[999] = '\0';
    
    // Concatenation
    char* concat = string_concat(str1, str2);
    
    // Reversal
    string_reverse(concat);
    
    // Pattern matching
    int matches = pattern_match(concat, "abc");
    
    // Result
    printf("%d\n", matches + (int)strlen(concat));
    
    free(concat);
    return 0;
}
