# LeetCode Problems in Sapphire

## Problem 1250: Check If It Is a Good Array

### Problem Statement

Given an array `nums` of positive integers, your task is to select some subset of `nums`, multiply each element by an integer and add all these numbers. The array is said to be **good** if you can obtain a sum of 1 from the array by any possible subset and multiplicand.

Return `true` if the array is good, otherwise return `false`.

### Examples

**Example 1:**
```
Input: nums = [12, 5, 7, 23]
Output: true
Explanation: Pick numbers 5 and 7.
5*3 + 7*(-2) = 15 - 14 = 1
```

**Example 2:**
```
Input: nums = [29, 6, 10]
Output: true
Explanation: Pick numbers 29, 6 and 10.
29*1 + 6*(-3) + 10*(-1) = 29 - 18 - 10 = 1
```

**Example 3:**
```
Input: nums = [3, 6]
Output: false
Explanation: No combination can produce 1 because GCD(3,6) = 3
```

### Solution Approach

This problem is based on **Bézout's Identity** from number theory:

> For integers a₁, a₂, ..., aₙ, there exist integers x₁, x₂, ..., xₙ such that:
> a₁x₁ + a₂x₂ + ... + aₙxₙ = gcd(a₁, a₂, ..., aₙ)

**Key Insight**: We can obtain a sum of 1 if and only if the GCD of all numbers is 1.

### Algorithm

1. Calculate the GCD of all elements in the array
2. If GCD = 1, return `true`
3. Otherwise, return `false`

### Complexity Analysis

- **Time Complexity**: O(n × log(max(nums)))
  - n iterations to process all elements
  - Each GCD calculation takes O(log(max(a,b))) time
  
- **Space Complexity**: O(1)
  - Only storing the running GCD value

### Implementation

#### C Implementation

```c
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

bool isGoodArray(int* nums, int numsSize) {
    int result = nums[0];
    for (int i = 1; i < numsSize; i++) {
        result = gcd(result, nums[i]);
        if (result == 1) return true;  // Early termination
    }
    return result == 1;
}
```

#### Sapphire Implementation (Conceptual)

```sapphire
# Note: Full implementation requires function support
# This shows the algorithm logic

fn gcd(a: int, b: int) -> int:
    while b != 0:
        let temp = b
        b = a % b
        a = temp
    return a

fn isGoodArray(nums: Vec<int>) -> bool:
    let result = nums[0]
    for num in nums[1:]:
        result = gcd(result, num)
        if result == 1:
            return true
    return result == 1
```

### Test Cases

| Input | Output | Explanation |
|-------|--------|-------------|
| `[12, 5, 7, 23]` | `true` | GCD(12,5,7,23) = 1 |
| `[29, 6, 10]` | `true` | GCD(29,6,10) = 1 |
| `[3, 6]` | `false` | GCD(3,6) = 3 ≠ 1 |
| `[1]` | `true` | GCD(1) = 1 |
| `[2, 4, 6, 8]` | `false` | GCD = 2 (all even) |
| `[7, 14, 21]` | `false` | GCD = 7 (all multiples of 7) |
| `[10, 15, 21]` | `true` | GCD = 1 (coprime) |

### Mathematical Proof

**Why GCD = 1 is necessary and sufficient:**

1. **Necessary**: If we can get sum = 1, then by Bézout's identity, GCD must divide 1, so GCD = 1.

2. **Sufficient**: If GCD = 1, then by Bézout's identity, there exist integers x₁, x₂, ..., xₙ such that:
   ```
   a₁x₁ + a₂x₂ + ... + aₙxₙ = 1
   ```

### GCD Calculation Examples

**Example 1: [12, 5, 7, 23]**
```
GCD(12, 5):
  12 = 5×2 + 2
  5 = 2×2 + 1
  2 = 1×2 + 0
  → GCD = 1

GCD(1, 7) = 1
GCD(1, 23) = 1
Final GCD = 1 ✓
```

**Example 2: [3, 6]**
```
GCD(3, 6):
  6 = 3×2 + 0
  → GCD = 3

Final GCD = 3 ≠ 1 ✗
```

### Running the Code

```bash
# Compile and run C version
gcc examples/leetcode/good_array.c -o good_array
./good_array

# Output:
# 1  (test case 1: true)
# 1  (test case 2: true)
# 0  (test case 3: false)
# 1  (test case 4: true)
# 0  (test case 5: false)
# 0  (test case 6: false)
# 1  (test case 7: true)
```

### Key Takeaways

1. **Number Theory**: This problem demonstrates practical application of GCD and Bézout's identity
2. **Optimization**: Early termination when GCD becomes 1
3. **Efficiency**: O(n log m) is very efficient even for large arrays
4. **Simplicity**: Complex problem reduces to simple GCD calculation

### Related Problems

- **LeetCode 914**: X of a Kind in a Deck of Cards (also uses GCD)
- **LeetCode 1071**: Greatest Common Divisor of Strings
- **LeetCode 365**: Water and Jug Problem (Bézout's identity)

### References

- [Bézout's Identity](https://en.wikipedia.org/wiki/B%C3%A9zout%27s_identity)
- [Euclidean Algorithm](https://en.wikipedia.org/wiki/Euclidean_algorithm)
- [Extended Euclidean Algorithm](https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)
