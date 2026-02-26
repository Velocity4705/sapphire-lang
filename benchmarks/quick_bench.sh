#!/bin/bash

# Quick Sapphire Performance Test

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         SAPPHIRE QUICK PERFORMANCE TEST                      ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Test 1: Interpreter mode
echo "Test 1: Interpreter Mode"
echo "Running arithmetic benchmark..."
time ./sapp benchmarks/bench_arithmetic.spp > /dev/null
echo ""

# Test 2: Compiled mode
echo "Test 2: Compiled Mode (LLVM -O3)"
echo "Compiling to native code..."
./sapp compile benchmarks/bench_arithmetic.spp > /tmp/bench.ll 2>&1
llc /tmp/bench.ll -o /tmp/bench.s -O3 2>/dev/null
clang /tmp/bench.s -o /tmp/bench_sapp -O3 2>/dev/null

echo "Running compiled Sapphire..."
time /tmp/bench_sapp > /dev/null
echo ""

# Test 3: C comparison
echo "Test 3: C Comparison (GCC -O3)"
echo "Compiling C version..."
gcc benchmarks/bench_arithmetic.c -o /tmp/bench_c -O3 -lm 2>/dev/null

echo "Running C version..."
time /tmp/bench_c > /dev/null
echo ""

# Test 4: Memory benchmark
echo "Test 4: Memory Operations"
echo "Sapphire (compiled):"
./sapp compile benchmarks/bench_memory.spp > /tmp/bench_mem.ll 2>&1
llc /tmp/bench_mem.ll -o /tmp/bench_mem.s -O3 2>/dev/null
clang /tmp/bench_mem.s -o /tmp/bench_mem_sapp -O3 2>/dev/null
time /tmp/bench_mem_sapp > /dev/null

echo ""
echo "C (compiled):"
gcc benchmarks/bench_memory.c -o /tmp/bench_mem_c -O3 2>/dev/null
time /tmp/bench_mem_c > /dev/null

echo ""
echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Note: Compiled Sapphire should be within 0-10% of C speed   ║"
echo "╚══════════════════════════════════════════════════════════════╝"
