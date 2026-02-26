# JIT Compilation

Sapphire features a sophisticated JIT (Just-In-Time) compiler that optimizes code at runtime for maximum performance.

## Overview

The JIT compiler:
- Starts with fast interpretation
- Profiles code execution
- Compiles hot paths to native code
- Applies runtime optimizations
- Adapts to actual usage patterns

## Enabling JIT

### Function-Level JIT

```sapphire
#[jit]
fn fibonacci(n: i64) -> i64 {
    if n <= 1 {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}
```

### With Optimization Hints

```sapphire
#[jit(inline_threshold: 100, unroll_loops: true)]
fn matrix_multiply(a: Matrix, b: Matrix) -> Matrix {
    // JIT will inline small functions and unroll loops
}
```

## Tiered Compilation

Sapphire uses a multi-tier compilation strategy:

1. **Tier 0**: Interpreter (fast startup)
2. **Tier 1**: Quick JIT (basic optimizations)
3. **Tier 2**: Optimizing JIT (aggressive optimizations)

```sapphire
#[tiered_compilation]
fn process_data(data: Vec<i64>) -> i64 {
    // Starts interpreted
    // Compiles to Tier 1 after ~100 calls
    // Compiles to Tier 2 after ~1000 calls
    let sum = 0
    for x in data {
        sum += x * x
    }
    return sum
}
```

## Profile-Guided Optimization

The JIT collects runtime statistics to guide optimization.

```sapphire
#[pgo]
fn search(arr: Vec<i64>, target: i64) -> Option<usize> {
    // JIT tracks:
    // - Branch frequencies
    // - Type distributions
    // - Call patterns
    for i in 0..arr.len() {
        if arr[i] == target {
            return Some(i)
        }
    }
    return None
}
```

## Adaptive Compilation

The JIT adapts to runtime behavior.

```sapphire
#[adaptive]
fn sort(arr: Vec<i64>) -> Vec<i64> {
    // JIT chooses algorithm based on:
    // - Array size
    // - Data distribution
    // - Hardware capabilities
    
    if arr.len() < 10 {
        return insertion_sort(arr)
    } else if is_nearly_sorted(arr) {
        return tim_sort(arr)
    } else {
        return quick_sort(arr)
    }
}
```

## Speculative Optimization

The JIT makes assumptions and optimizes speculatively.

```sapphire
#[speculative]
fn polymorphic_call(obj: Any) -> i64 {
    // JIT assumes obj is usually the same type
    // Generates fast path for common case
    // Falls back to slow path if assumption fails
    return obj.compute()
}
```

## Inline Caching

Optimize dynamic dispatch with inline caching.

```sapphire
#[inline_cache]
fn make_sound(animal: Animal) {
    // First call: polymorphic dispatch (slow)
    // Subsequent calls: cached monomorphic dispatch (fast)
    print(animal.speak())
}
```

## Loop Optimizations

### Vectorization

```sapphire
#[jit(vectorize: true)]
fn sum_array(arr: Vec<f64>) -> f64 {
    let sum = 0.0
    // JIT vectorizes using SIMD instructions
    for x in arr {
        sum += x
    }
    return sum
}
```

### Loop Unrolling

```sapphire
#[jit(unroll: 4)]
fn process_elements(arr: Vec<i64>) {
    // JIT unrolls loop 4 times
    for x in arr {
        process(x)
    }
}
```

## Escape Analysis

The JIT determines if objects escape their scope.

```sapphire
#[jit(escape_analysis: true)]
fn create_point(x: f64, y: f64) -> Point {
    // JIT allocates on stack if Point doesn't escape
    let p = Point { x, y }
    return p
}
```

## Deoptimization

The JIT can deoptimize when assumptions are violated.

```sapphire
#[jit(deopt_on_type_change: true)]
fn process_value(value: Any) -> i64 {
    // Optimized for specific type
    // Deoptimizes if type changes
    return value.to_int() * 2
}
```

## Branch Prediction

Provide hints for branch prediction.

```sapphire
fn predict_branch(condition: bool) {
    if likely!(condition) {
        // Hot path - JIT optimizes this
        fast_operation()
    } else {
        // Cold path
        slow_operation()
    }
}

fn error_handling(result: Result) {
    if unlikely!(result.is_error()) {
        handle_error()
    } else {
        continue_processing()
    }
}
```

## Runtime Code Generation

Generate and compile code at runtime.

```sapphire
fn create_specialized_function(operation: String) -> fn(i64, i64) -> i64 {
    let code = match operation {
        "add" => "fn(a, b) { return a + b }",
        "mul" => "fn(a, b) { return a * b }",
        _ => "fn(a, b) { return 0 }"
    }
    
    // JIT compile the generated code
    return jit::compile(code)
}

let add_fn = create_specialized_function("add")
let result = add_fn(5, 3)  // 8
```

## JIT Control

### Manual Control

```sapphire
fn manual_jit_example() {
    // Disable JIT
    jit::disable()
    debug_code()
    
    // Enable JIT
    jit::enable()
    
    // Force compilation
    jit::compile_function(hot_function)
    
    // Warm up JIT
    for _ in 0..1000 {
        hot_function()
    }
}
```

### JIT Statistics

```sapphire
fn print_jit_stats() {
    let stats = jit::get_stats()
    print(f"Compiled functions: {stats.compiled_count}")
    print(f"Compilation time: {stats.compile_time_ms}ms")
    print(f"Speedup: {stats.speedup}x")
    print(f"Deoptimizations: {stats.deopt_count}")
}
```

## Optimization Levels

```sapphire
// No optimization
#[jit(opt_level: 0)]
fn debug_function() { }

// Basic optimization
#[jit(opt_level: 1)]
fn normal_function() { }

// Aggressive optimization
#[jit(opt_level: 2)]
fn hot_function() { }

// Maximum optimization
#[jit(opt_level: 3)]
fn critical_function() { }
```

## Benchmarking

Compare JIT vs interpreted performance.

```sapphire
fn benchmark_jit() {
    let data = Vec::range(0, 1000000)
    
    // Interpreted mode
    jit::disable()
    let start = time::now()
    let result1 = process_data(data)
    let interpreted_time = time::now() - start
    
    // JIT mode
    jit::enable()
    let start = time::now()
    let result2 = process_data(data)
    let jit_time = time::now() - start
    
    print(f"Interpreted: {interpreted_time}ms")
    print(f"JIT: {jit_time}ms")
    print(f"Speedup: {interpreted_time / jit_time}x")
}
```

## Best Practices

1. **Let JIT decide** - Don't micro-optimize prematurely
2. **Profile first** - Identify hot paths before optimizing
3. **Use hints sparingly** - JIT is usually smart enough
4. **Warm up critical paths** - Run hot code before measuring
5. **Monitor deoptimizations** - Too many indicate problems

## Performance Tips

### Write JIT-Friendly Code

```sapphire
// Good - monomorphic
fn process_int(x: i64) -> i64 {
    return x * 2
}

// Bad - polymorphic (harder to optimize)
fn process_any(x: Any) -> Any {
    return x * 2
}
```

### Avoid Type Instability

```sapphire
// Good - stable types
fn stable_types() {
    let x: i64 = 0
    for i in 0..100 {
        x += i  // x is always i64
    }
}

// Bad - type changes
fn unstable_types() {
    let x = 0
    for i in 0..100 {
        if i % 2 == 0 {
            x = i  // i64
        } else {
            x = i as f64  // f64 - type changed!
        }
    }
}
```

### Keep Functions Small

```sapphire
// Good - small, inlinable
fn add(a: i64, b: i64) -> i64 {
    return a + b
}

// Bad - too large to inline
fn huge_function() {
    // 1000 lines of code...
}
```

## Debugging JIT

### Trace Compilation

```bash
sapp --trace-jit file.spp
```

### Dump Generated Code

```bash
sapp --dump-jit-code file.spp
```

### Profile JIT

```bash
sapp --profile-jit file.spp
```

## Architecture

Sapphire's JIT uses:
- **LLVM** for code generation
- **Tiered compilation** for fast startup
- **Profile-guided optimization** for adaptive behavior
- **Inline caching** for dynamic dispatch
- **Escape analysis** for stack allocation
- **SIMD** for vectorization

## Performance

Typical speedups:
- **2-5x** for numeric code
- **5-10x** for loops
- **10-20x** for hot paths
- **20-50x** for specialized code

## Examples

See `examples/jit_compilation.spp` for complete examples.

## Further Reading

- [Performance Guide](../PERFORMANCE.md)
- [Optimization Strategies](OPTIMIZATION.md)
- [Benchmarking](../benchmarks/README.md)
