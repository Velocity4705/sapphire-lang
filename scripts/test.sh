#!/bin/bash
# Run Sapphire test suite

set -e

cd build

echo "Running unit tests..."
ctest --output-on-failure -R unit

echo "Running integration tests..."
ctest --output-on-failure -R integration

echo "Running end-to-end tests..."
ctest --output-on-failure -R e2e

echo "Running benchmarks..."
./tests/benchmarks/bench_fibonacci
./tests/benchmarks/bench_sorting

echo "✓ All tests passed!"
