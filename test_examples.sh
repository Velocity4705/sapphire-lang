#!/bin/bash
# Test all working examples to prevent first-day errors

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║         Testing Sapphire Working Examples                    ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

PASS=0
FAIL=0

# List of working examples
EXAMPLES=(
    "examples/hello.spp"
    "examples/simple.spp"
    "examples/showcase.spp"
    "examples/data_science_simple.spp"
    "examples/test_basic.spp"
    "examples/test_arithmetic.spp"
    "examples/test_comparison.spp"
    "examples/file_io_example.spp"
    "examples/user_input_example.spp"
    "examples/system_programming_example.spp"
    "examples/directives_example.spp"
    "examples/codegen_test.spp"
    "examples/comprehensive_test.spp"
    "examples/concurrent_example.spp"
    "examples/expr_test.spp"
    "examples/float_test.spp"
    "examples/json_example.spp"
    "examples/showcase_compiler.spp"
    "examples/simple_test.spp"
    "examples/test_print_advanced.spp"
    "examples/test_print_compile.spp"
    "examples/test_type_promotion.spp"
    "examples/test_with_print.spp"
    "examples/type_test.spp"
    "examples/control_flow_working.spp"
    "examples/functions_working.spp"
    "examples/arrays_working.spp"
    "examples/file_io_working.spp"
    "examples/complete_io_demo.spp"
)

for example in "${EXAMPLES[@]}"; do
    echo "Testing: $example"
    
    # Run the example and capture output
    output=$(./sapp "$example" 2>&1)
    exit_code=$?
    
    # Check for parse errors
    if echo "$output" | grep -q "Parse error"; then
        echo "  ✗ FAIL - Parse errors found"
        echo "$output" | grep "Parse error" | head -3
        FAIL=$((FAIL + 1))
    elif [ $exit_code -ne 0 ]; then
        echo "  ✗ FAIL - Exit code: $exit_code"
        FAIL=$((FAIL + 1))
    else
        echo "  ✓ PASS"
        PASS=$((PASS + 1))
    fi
    echo ""
done

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║  Test Results                                                 ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""
echo "Passed: $PASS"
echo "Failed: $FAIL"
echo "Total:  $((PASS + FAIL))"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "✓ All examples working! Ready for launch! 🚀"
    exit 0
else
    echo "✗ Some examples failed. Fix before launch."
    exit 1
fi
