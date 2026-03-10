#!/bin/bash

# Sapphire Beta.3 - Complete Examples Test Suite
# Tests all educational examples to verify language functionality

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║              SAPPHIRE BETA.3 - EXAMPLES TEST SUITE           ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

# Check if sapphire compiler exists
if [ ! -f "./sapp" ]; then
    echo -e "${RED}Error: ./sapp not found. Please build Sapphire first.${NC}"
    exit 1
fi

# Create results directory
mkdir -p test_results
RESULTS_FILE="test_results/beta3_examples_results.txt"
echo "Sapphire Beta.3 Examples Test Results" > $RESULTS_FILE
echo "Started: $(date)" >> $RESULTS_FILE
echo "========================================" >> $RESULTS_FILE

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a single example
run_example() {
    local file=$1
    local description=$2
    
    echo -e "${BLUE}Testing: ${file}${NC}"
    if [ -n "$description" ]; then
        echo -e "${YELLOW}  Description: $description${NC}"
    fi
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    # Run the example and capture output
    if timeout 10s ./sapp "$file" > "test_results/$(basename $file .spp)_output.txt" 2>&1; then
        echo -e "${GREEN}  ✓ PASSED${NC}"
        echo "✓ PASSED: $file" >> $RESULTS_FILE
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}  ✗ FAILED${NC}"
        echo "✗ FAILED: $file" >> $RESULTS_FILE
        FAILED_TESTS=$((FAILED_TESTS + 1))
        
        # Show error details
        echo "    Error output:" >> $RESULTS_FILE
        tail -5 "test_results/$(basename $file .spp)_output.txt" >> $RESULTS_FILE
        echo "" >> $RESULTS_FILE
    fi
    echo ""
}

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                    CORE LANGUAGE EXAMPLES                      ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Basic Examples
run_example "examples/hello.spp" "Hello World - Basic output"
run_example "examples/simple.spp" "Simple variables and arithmetic"
run_example "examples/showcase.spp" "Language features showcase"
run_example "examples/demo.spp" "General language demonstration"
run_example "examples/fibonacci.spp" "Fibonacci algorithm"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                   PATTERN MATCHING EXAMPLES                    ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Pattern Matching
run_example "examples/pattern_matching_basic.spp" "Basic pattern matching"
run_example "examples/pattern_matching_comprehensive.spp" "Comprehensive pattern matching"
run_example "examples/pattern_matching_simple.spp" "Simple pattern matching"
run_example "examples/pattern_matching_arrays.spp" "Array pattern matching"
run_example "examples/pattern_matching_guards.spp" "Pattern matching with guards"
run_example "examples/pattern_matching_multiple.spp" "Multiple pattern matching"
run_example "examples/pattern_matching_objects.spp" "Object pattern matching"
run_example "examples/pattern_matching_rest.spp" "Rest pattern matching"
run_example "examples/match_guard_simple.spp" "Simple guard clauses"
run_example "examples/match_guard_oneline.spp" "One-line guard clauses"
run_example "examples/match_guard_indent.spp" "Indented guard clauses"
run_example "examples/match_multi_arm.spp" "Multi-arm matching"
run_example "examples/match_multiple_patterns.spp" "Multiple patterns per arm"
run_example "examples/match_no_main.spp" "Pattern matching without main"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                      TRAITS EXAMPLES                          ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Traits
run_example "examples/traits_simple.spp" "Simple traits"
run_example "examples/traits_complete.spp" "Complete traits system"
run_example "examples/traits_working.spp" "Working traits implementation"
run_example "examples/traits_impl.spp" "Trait implementations"
run_example "examples/traits_multiple.spp" "Multiple traits"
run_example "examples/traits_generics.spp" "Traits with generics"
run_example "examples/traits_full_demo.spp" "Full traits demonstration"
run_example "examples/trait_minimal.spp" "Minimal trait example"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                    DECORATORS EXAMPLES                        ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Decorators
run_example "examples/decorators_complete.spp" "Complete decorators system"
run_example "examples/decorators_comprehensive.spp" "Comprehensive decorators"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                   CONCURRENCY EXAMPLES                        ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Concurrency & Async
run_example "examples/async_simple.spp" "Simple async/await"
run_example "examples/async_comprehensive.spp" "Comprehensive async"
run_example "examples/concurrent_example.spp" "Concurrent programming"
run_example "examples/concurrency_complete.spp" "Complete concurrency"
run_example "examples/concurrency_showcase.spp" "Concurrency showcase"
run_example "examples/channel_simple.spp" "Simple channels"
run_example "examples/channel_comprehensive.spp" "Comprehensive channels"
run_example "examples/channel_methods.spp" "Channel methods"
run_example "examples/channel_iteration.spp" "Channel iteration"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                  OPTION/RESULT EXAMPLES                       ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Option Types
run_example "examples/option_simple.spp" "Simple Option types"
run_example "examples/option_minimal.spp" "Minimal Option example"
run_example "examples/option_comprehensive.spp" "Comprehensive Option"
run_example "examples/option_showcase.spp" "Option showcase"
run_example "examples/option_practical.spp" "Practical Option usage"
run_example "examples/option_none_var.spp" "Option None variables"

# Result Types
run_example "examples/result_simple.spp" "Simple Result types"
run_example "examples/result_comprehensive.spp" "Comprehensive Result"
run_example "examples/result_showcase.spp" "Result showcase"
run_example "examples/result_practical.spp" "Practical Result usage"
run_example "examples/result_compare.spp" "Result comparison"
run_example "examples/result_validate.spp" "Result validation"
run_example "examples/result_chain.spp" "Result chaining"
run_example "examples/result_chain2.spp" "Result chaining 2"
run_example "examples/result_chain3.spp" "Result chaining 3"

# Try Operator
run_example "examples/try_operator_simple.spp" "Simple try operator"
run_example "examples/try_operator_showcase.spp" "Try operator showcase"
run_example "examples/try_operator_practical.spp" "Practical try operator"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                   STANDARD LIBRARY EXAMPLES                   ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Standard Library
run_example "examples/stdlib_complete_demo.spp" "Complete stdlib demo"
run_example "examples/stdlib_comprehensive.spp" "Comprehensive stdlib"
run_example "examples/stdlib_collections.spp" "Collections library"
run_example "examples/stdlib_itertools.spp" "Itertools library"
run_example "examples/stdlib_timing.spp" "Timing utilities"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                      CONTROL FLOW EXAMPLES                    ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Control Flow
run_example "examples/control_flow_working.spp" "Working control flow"
run_example "examples/functions_working.spp" "Working functions"
run_example "examples/arrays_working.spp" "Working arrays"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                        I/O EXAMPLES                           ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# I/O and System
run_example "examples/file_io_example.spp" "File I/O operations"
run_example "examples/file_io_working.spp" "Working file I/O"
run_example "examples/complete_io_demo.spp" "Complete I/O demo"
run_example "examples/user_input_example.spp" "User input handling"
run_example "examples/input_working.spp" "Working input"
run_example "examples/json_example.spp" "JSON processing"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                    ADVANCED EXAMPLES                          ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Advanced Features
run_example "examples/modules_working.spp" "Working modules"
run_example "examples/modules_practical.spp" "Practical modules"
run_example "examples/oop_working.spp" "Working OOP"
run_example "examples/oop_practical.spp" "Practical OOP"
run_example "examples/error_handling_working.spp" "Working error handling"
run_example "examples/performance_showcase.spp" "Performance showcase"
run_example "examples/system_programming_example.spp" "System programming"
run_example "examples/directives_example.spp" "Compiler directives"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                      MACRO EXAMPLES                           ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Macros
run_example "examples/macro_minimal.spp" "Minimal macros"
run_example "examples/macros_simple.spp" "Simple macros"
run_example "examples/macros_basic.spp" "Basic macros"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                    HOT RELOAD EXAMPLES                        ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Hot Reload
run_example "examples/hot_reload_demo.spp" "Hot reload demo"
run_example "examples/hot_reload_comprehensive.spp" "Comprehensive hot reload"
run_example "examples/reload_demo.spp" "Reload demonstration"

echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                     DOMAIN EXAMPLES                           ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Domain-Specific
run_example "examples/data_science_simple.spp" "Data science operations"
run_example "examples/showcase_compiler.spp" "Compiler showcase"
run_example "examples/sapphire.spp" "Sapphire language demo"

echo ""
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}                        TEST SUMMARY                           ${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"

# Final Results
echo "" >> $RESULTS_FILE
echo "========================================" >> $RESULTS_FILE
echo "FINAL RESULTS:" >> $RESULTS_FILE
echo "Total Tests: $TOTAL_TESTS" >> $RESULTS_FILE
echo "Passed: $PASSED_TESTS" >> $RESULTS_FILE
echo "Failed: $FAILED_TESTS" >> $RESULTS_FILE
echo "Success Rate: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%" >> $RESULTS_FILE
echo "Completed: $(date)" >> $RESULTS_FILE

echo -e "${BLUE}Total Examples Tested: $TOTAL_TESTS${NC}"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL EXAMPLES PASSED! Sapphire Beta.3 is ready! 🎉${NC}"
    SUCCESS_RATE=100
else
    SUCCESS_RATE=$(( PASSED_TESTS * 100 / TOTAL_TESTS ))
    echo -e "${YELLOW}Success Rate: ${SUCCESS_RATE}%${NC}"
    
    if [ $SUCCESS_RATE -ge 90 ]; then
        echo -e "${GREEN}✅ Excellent! Beta.3 is in great shape!${NC}"
    elif [ $SUCCESS_RATE -ge 80 ]; then
        echo -e "${YELLOW}⚠️  Good, but some issues need attention.${NC}"
    else
        echo -e "${RED}❌ Significant issues found. Review needed.${NC}"
    fi
fi

echo ""
echo -e "${BLUE}Detailed results saved to: $RESULTS_FILE${NC}"
echo -e "${BLUE}Individual outputs saved to: test_results/[example]_output.txt${NC}"
echo ""

# Return appropriate exit code
if [ $FAILED_TESTS -eq 0 ]; then
    exit 0
else
    exit 1
fi