#!/bin/bash

# Test LeetCode 1250: Check If It Is a Good Array

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  LeetCode 1250: Check If It Is a Good Array                  ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

# Compile C version
echo "Compiling C implementation..."
gcc examples/leetcode/good_array.c -o /tmp/good_array_test
echo ""

# Run tests
echo "Running test cases..."
echo ""

output=$(/tmp/good_array_test)

# Parse output
IFS=$'\n' read -d '' -r -a results <<< "$output"

# Test cases
declare -a test_cases=(
    "[12, 5, 7, 23]|1|GCD=1, coprime numbers"
    "[29, 6, 10]|1|GCD=1, coprime numbers"
    "[3, 6]|0|GCD=3, not coprime"
    "[1]|1|Single element 1"
    "[2, 4, 6, 8]|0|All even numbers, GCD=2"
    "[7, 14, 21]|0|All multiples of 7, GCD=7"
    "[10, 15, 21]|1|GCD=1, coprime"
)

passed=0
failed=0

for i in "${!test_cases[@]}"; do
    IFS='|' read -r input expected explanation <<< "${test_cases[$i]}"
    actual="${results[$i]}"
    
    echo -e "${BLUE}Test $((i+1)):${NC} $input"
    echo "  Expected: $expected"
    echo "  Actual:   $actual"
    echo "  Reason:   $explanation"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "  ${GREEN}✓ PASS${NC}"
        ((passed++))
    else
        echo -e "  ${RED}✗ FAIL${NC}"
        ((failed++))
    fi
    echo ""
done

# Summary
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║                      TEST SUMMARY                            ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo -e "Tests Passed: ${GREEN}$passed${NC}"
echo -e "Tests Failed: ${RED}$failed${NC}"
echo ""

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    echo ""
    echo "Algorithm Details:"
    echo "  • Time Complexity: O(n × log(max(nums)))"
    echo "  • Space Complexity: O(1)"
    echo "  • Based on Bézout's Identity"
    echo "  • GCD = 1 ⟺ Array is good"
    exit 0
else
    echo -e "${RED}✗ Some tests failed!${NC}"
    exit 1
fi
