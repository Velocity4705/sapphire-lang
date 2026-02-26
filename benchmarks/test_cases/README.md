# Algorithm Benchmark Test Cases

## Overview

Each algorithm benchmark has documented test cases with inputs and expected outputs for verification.

## Test Case Format

```
Algorithm: [Name]
Input: [Description of input]
Expected Output: [Expected result]
Actual Output: [Run to verify]
Time Complexity: [Big O notation]
Space Complexity: [Big O notation]
```

## Running Tests

```bash
# Run individual test
./sapp benchmarks/algorithms/[algorithm].spp

# Run all tests with verification
./benchmarks/verify_outputs.sh

# Compare with C implementation
./benchmarks/compare_with_c.sh
```

## Test Cases

### 1. Fibonacci (Dynamic Programming)

**Input**: n = 40
**Expected Output**: 102334155
**Time Complexity**: O(n)
**Space Complexity**: O(n)

**Verification**:
```bash
./sapp benchmarks/algorithms/fibonacci_dp.spp
# Should output: 102334155
```

### 2. Matrix Multiplication

**Input**: 
- Matrix A (3x3): [[1,2,3], [4,5,6], [7,8,9]]
- Matrix B (3x3): [[9,8,7], [6,5,4], [3,2,1]]

**Expected Output**: Sum of result matrix = 621
**Time Complexity**: O(n³)
**Space Complexity**: O(n²)

**Calculation**:
```
C[0][0] = 1*9 + 2*6 + 3*3 = 9 + 12 + 9 = 30
C[0][1] = 1*8 + 2*5 + 3*2 = 8 + 10 + 6 = 24
C[0][2] = 1*7 + 2*4 + 3*1 = 7 + 8 + 3 = 18
C[1][0] = 4*9 + 5*6 + 6*3 = 36 + 30 + 18 = 84
C[1][1] = 4*8 + 5*5 + 6*2 = 32 + 25 + 12 = 69
C[1][2] = 4*7 + 5*4 + 6*1 = 28 + 20 + 6 = 54
C[2][0] = 7*9 + 8*6 + 9*3 = 63 + 48 + 27 = 138
C[2][1] = 7*8 + 8*5 + 9*2 = 56 + 40 + 18 = 114
C[2][2] = 7*7 + 8*4 + 9*1 = 49 + 32 + 9 = 90

Sum = 30+24+18+84+69+54+138+114+90 = 621
```

**Verification**:
```bash
./sapp benchmarks/algorithms/matrix_multiply.spp
# Should output: 621
```

### 3. Hash Operations

**Input**: 
- Keys: [12345, 67890, 11111, 22222, 33333]
- Hash function: key % 1000

**Expected Output**: 3802

**Calculation**:
```
hash1 = 12345 % 1000 = 345
hash2 = 67890 % 1000 = 890
hash3 = 11111 % 1000 = 111
hash4 = 22222 % 1000 = 222
hash5 = 33333 % 1000 = 333

val1 = 345 * 2 = 690
val2 = 890 * 2 = 1780
val3 = 111 * 2 = 222
val4 = 222 * 2 = 444
val5 = 333 * 2 = 666

result = 690 + 1780 + 222 + 444 + 666 = 3802
```

**Time Complexity**: O(1) average per operation
**Space Complexity**: O(n)

**Verification**:
```bash
./sapp benchmarks/algorithms/hash_operations.spp
# Should output: 3802
```

### 4. String Operations

**Input**: 
- String length: 26
- Character range: 'a' to 'z' (97 to 122)
- Substring: positions 5 to 15
- Concatenation: strings of length 10 and 15
- Pattern matching: pattern length 5, text length 100

**Expected Output**: 181

**Calculation**:
```
str_len = 26
char_range = 122 - 97 = 25
substr_len = 15 - 5 = 10
concat_len = 10 + 15 = 25
max_comparisons = 100 - 5 = 95

result = 26 + 25 + 10 + 25 + 95 = 181
```

**Time Complexity**: O(n) for most operations
**Space Complexity**: O(n)

**Verification**:
```bash
./sapp benchmarks/algorithms/string_operations.spp
# Should output: 181
```

### 5. Sorting (Bubble Sort)

**Input**: Array [64, 34, 25, 12, 22, 11, 90, 88, 45, 50]

**Expected Output**: Sum of array = 441

**Calculation**:
```
Original: [64, 34, 25, 12, 22, 11, 90, 88, 45, 50]
Sum = 64 + 34 + 25 + 12 + 22 + 11 + 90 + 88 + 45 + 50 = 441
```

**Time Complexity**: O(n²)
**Space Complexity**: O(1)

**Verification**:
```bash
./sapp benchmarks/algorithms/sorting_benchmark.spp
# Should output: 441
```

## Verification Script

Run all tests and verify outputs:

```bash
./benchmarks/verify_outputs.sh
```

Expected output:
```
Testing Fibonacci DP...
  Expected: 102334155
  Actual: 102334155
  ✓ PASS

Testing Matrix Multiply...
  Expected: 621
  Actual: 621
  ✓ PASS

Testing Hash Operations...
  Expected: 3802
  Actual: 3802
  ✓ PASS

Testing String Operations...
  Expected: 181
  Actual: 181
  ✓ PASS

Testing Sorting...
  Expected: 441
  Actual: 441
  ✓ PASS

All tests passed! ✓
```

## Performance Comparison

Compare Sapphire vs C performance:

```bash
./benchmarks/compare_with_c.sh
```

Expected output format:
```
Algorithm: Fibonacci DP
  Sapphire: 0.123ms
  C:        0.120ms
  Ratio:    102.5% (within 5% target)
  ✓ PASS
```

## Adding New Test Cases

To add a new algorithm benchmark:

1. Create `benchmarks/algorithms/your_algorithm.spp`
2. Create `benchmarks/algorithms/your_algorithm.c`
3. Add test case to this README
4. Update `verify_outputs.sh`
5. Document input, output, and complexity

## Test Case Template

```markdown
### N. Algorithm Name

**Input**: [Describe input]
**Expected Output**: [Expected result]
**Time Complexity**: O(?)
**Space Complexity**: O(?)

**Calculation**:
```
[Show step-by-step calculation]
```

**Verification**:
```bash
./sapp benchmarks/algorithms/your_algorithm.spp
# Should output: [expected]
```
```

## Notes

- All test cases are deterministic (no random input)
- Outputs are verified against mathematical calculations
- C implementations use same algorithms for fair comparison
- Complexity analysis is theoretical and verified empirically
