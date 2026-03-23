#!/bin/bash

# Sapphire Performance Benchmark Suite
# Compares Sapphire performance against C

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         SAPPHIRE 1.0-beta.6 PERFORMANCE BENCHMARKS           ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Create output directory
mkdir -p benchmarks/output

# Function to run benchmark
run_benchmark() {
    local name=$1
    local spp_file=$2
    local c_file=$3
    
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${YELLOW}Benchmark: $name${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    
    # Compile Sapphire to LLVM IR
    echo "  [1/5] Compiling Sapphire to LLVM IR..."
    ./sapp compile "$spp_file" > "benchmarks/output/${name}.ll" 2>&1
    
    # Compile LLVM IR to object file
    echo "  [2/5] Compiling LLVM IR to native code..."
    llc "benchmarks/output/${name}.ll" -o "benchmarks/output/${name}.s" -O3
    clang "benchmarks/output/${name}.s" -o "benchmarks/output/${name}_sapp" -O3
    
    # Compile C version
    echo "  [3/5] Compiling C version..."
    gcc "$c_file" -o "benchmarks/output/${name}_c" -O3 -lm
    
    # Run Sapphire version multiple times
    echo "  [4/5] Running Sapphire version (10 iterations)..."
    local sapp_total=0
    for i in {1..10}; do
        local start=$(date +%s%N)
        "benchmarks/output/${name}_sapp" > /dev/null
        local end=$(date +%s%N)
        local elapsed=$((end - start))
        sapp_total=$((sapp_total + elapsed))
    done
    local sapp_avg=$((sapp_total / 10))
    
    # Run C version multiple times
    echo "  [5/5] Running C version (10 iterations)..."
    local c_total=0
    for i in {1..10}; do
        local start=$(date +%s%N)
        "benchmarks/output/${name}_c" > /dev/null
        local end=$(date +%s%N)
        local elapsed=$((end - start))
        c_total=$((c_total + elapsed))
    done
    local c_avg=$((c_total / 10))
    
    # Calculate results
    local sapp_ms=$(echo "scale=3; $sapp_avg / 1000000" | bc)
    local c_ms=$(echo "scale=3; $c_avg / 1000000" | bc)
    local ratio=$(echo "scale=2; $sapp_avg * 100 / $c_avg" | bc)
    local diff=$(echo "scale=2; $ratio - 100" | bc)
    
    echo ""
    echo -e "${GREEN}Results:${NC}"
    echo "  Sapphire: ${sapp_ms} ms"
    echo "  C:        ${c_ms} ms"
    echo "  Ratio:    ${ratio}% of C speed"
    
    if (( $(echo "$diff < 5" | bc -l) )); then
        echo -e "  ${GREEN}✓ Within 5% of C performance!${NC}"
    elif (( $(echo "$diff < 10" | bc -l) )); then
        echo -e "  ${YELLOW}⚠ Within 10% of C performance${NC}"
    else
        echo -e "  ${RED}✗ More than 10% slower than C${NC}"
    fi
    echo ""
}

# Run benchmarks
run_benchmark "arithmetic" "benchmarks/bench_arithmetic.spp" "benchmarks/bench_arithmetic.c"
run_benchmark "memory" "benchmarks/bench_memory.spp" "benchmarks/bench_memory.c"

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║                    BENCHMARK COMPLETE                        ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo "Note: Benchmarks compare LLVM-optimized Sapphire (-O3) vs GCC C (-O3)"
echo "Results show Sapphire's compiled performance, not interpreter mode."
echo ""
