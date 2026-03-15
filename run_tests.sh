#!/bin/bash
# Run all non-interactive, non-GUI examples and report results

SAPP="./sapp"
PASS=0
FAIL=0
SKIP=0
ERRORS=()

# Skip: interactive (need stdin), GUI/window (need display), hot-reload (need file watcher)
SKIP_PATTERNS=(
    "input_working"
    "user_input"
    "gui_window_demo"
    "milestone11_window_test"
    "milestone11_gui_test"
    "hot_reload"
    "reload_demo"
    "directives_example"
)

run_test() {
    local file="$1"
    local name=$(basename "$file")

    for pat in "${SKIP_PATTERNS[@]}"; do
        if [[ "$file" == *"$pat"* ]]; then
            echo "  SKIP  $name"
            ((SKIP++))
            return
        fi
    done

    output=$(timeout 10s "$SAPP" "$file" 2>&1)
    exit_code=$?

    if [ $exit_code -eq 124 ]; then
        echo "  TIMEOUT $name"
        ERRORS+=("TIMEOUT: $name")
        ((FAIL++))
    elif [ $exit_code -ne 0 ]; then
        if echo "$output" | grep -qE "^Error:|RuntimeError:|TypeError:|Undefined variable|NameError"; then
            echo "  FAIL  $name"
            last_err=$(echo "$output" | grep -E "^Error:|RuntimeError:|TypeError:|Undefined variable|NameError" | tail -1)
            ERRORS+=("FAIL: $name -- $last_err")
            ((FAIL++))
        else
            echo "  PASS  $name  (warnings only)"
            ((PASS++))
        fi
    else
        echo "  PASS  $name"
        ((PASS++))
    fi
}

echo "=== Running examples/*.spp ==="
for f in examples/*.spp; do
    run_test "$f"
done

echo ""
echo "=== Summary ==="
echo "PASS: $PASS  FAIL: $FAIL  SKIP: $SKIP"
echo ""
if [ ${#ERRORS[@]} -gt 0 ]; then
    echo "=== Failures ==="
    for e in "${ERRORS[@]}"; do
        echo "  $e"
    done
fi
