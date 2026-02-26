#!/bin/bash

# Verify all algorithm benchmark outputs

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         ALGORITHM BENCHMARK OUTPUT VERIFICATION              ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

PASSED=0
FAILED=0

# Function to test algorithm
test_algorithm() {
    local name=$1
    local file=$2
    local expected=$3
    local complexity=$4
    
    echo -e "${BLUE}Testing: $name${NC}"
    echo "  Complexity: $complexity"
    echo -n "  Running... "
    
    # Run and capture output (filter out parse errors)
    local actual=$(./sapp "$file" 2>/dev/null | grep -E '^[0-9]+' | tail -1)
    
    echo "Done"
    echo "  Expected: $expected"
    echo "  Actual:   $actual"
    
    if [ "$actual" = "$expected" ]; then
        echo -e "  ${GREEN}✓ PASS${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "  ${RED}✗ FAIL${NC}"
        FAILED=$((FAILED + 1))
    fi
    echo ""
}

# Run all tests
echo "Running algorithm benchmarks..."
echo ""

test_algorithm \
    "Fibonacci DP" \
    "benchmarks/algorithms/fibonacci_dp.spp" \
    "55" \
    "Time: O(n), Space: O(n)"

test_algorithm \
    "Matrix Multiplication" \
    "benchmarks/algorithms/matrix_multiply.spp" \
    "621" \
    "Time: O(n³), Space: O(n²)"

test_algorithm \
    "Hash Operations" \
    "benchmarks/algorithms/hash_operations.spp" \
    "3802" \
    "Time: O(1) avg, Space: O(n)"

test_algorithm \
    "String Operations" \
    "benchmarks/algorithms/string_operations.spp" \
    "181" \
    "Time: O(n), Space: O(n)"

test_algorithm \
    "Sorting (Bubble Sort)" \
    "benchmarks/algorithms/sorting_benchmark.spp" \
    "441" \
    "Time: O(n²), Space: O(1)"

# Summary
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║                      TEST SUMMARY                            ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo -e "Tests Passed: ${GREEN}$PASSED${NC}"
echo -e "Tests Failed: ${RED}$FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
    echo ""
    echo "All algorithm outputs verified correctly."
    echo "Sapphire maintains correct algorithmic behavior."
    exit 0
else
    echo -e "${RED}✗ Some tests failed!${NC}"
    echo ""
    echo "Please check the failed tests above."
    exit 1
fi
