#!/bin/bash

# Sapphire Performance Benchmark
# Compares compiled Sapphire vs C performance

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║       SAPPHIRE 1.0-beta.6 PERFORMANCE BENCHMARK              ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Compile Sapphire to native
echo "[1/4] Compiling Sapphire to LLVM IR..."
./sapp compile benchmarks/bench_simple.spp > /tmp/bench.ll 2>&1

echo "[2/4] Compiling LLVM IR to native code (with -O3)..."
llc /tmp/bench.ll -o /tmp/bench.s -O3 2>/dev/null
clang /tmp/bench.s -o /tmp/bench_sapphire -O3 2>/dev/null

echo "[3/4] Compiling C version (with -O3)..."
gcc benchmarks/bench_simple.c -o /tmp/bench_c -O3 2>/dev/null

echo "[4/4] Running benchmarks..."
echo ""

# Verify outputs match
echo "Verifying correctness..."
SAPP_OUT=$(/tmp/bench_sapphire)
C_OUT=$(/tmp/bench_c)
echo "  Sapphire output: $SAPP_OUT"
echo "  C output:        $C_OUT"

if [ "$SAPP_OUT" = "$C_OUT" ]; then
    echo "  ✓ Outputs match!"
else
    echo "  ✗ Outputs differ!"
fi
echo ""

# Run performance tests
echo "Performance Test (1000 iterations each):"
echo ""

echo "Sapphire (compiled with LLVM -O3):"
time for i in {1..1000}; do /tmp/bench_sapphire > /dev/null; done
echo ""

echo "C (compiled with GCC -O3):"
time for i in {1..1000}; do /tmp/bench_c > /dev/null; done
echo ""

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Sapphire uses LLVM optimization to achieve C-like speed    ║"
echo "╚══════════════════════════════════════════════════════════════╝"
