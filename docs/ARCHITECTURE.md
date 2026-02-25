# Sapphire Architecture

## Compilation Pipeline

```
Source Code (.spp)
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST (Abstract Syntax Tree)
    ↓
[Semantic Analysis] → Type-checked AST
    ↓
[IR Generation] → Intermediate Representation
    ↓
[Optimization] → Optimized IR
    ↓
[Code Generation] → Native Code / Bytecode
    ↓
Executable
```

## Multi-Tier Execution Strategy

### Tier 1: Interpreter (Development)
- Fast startup, no compilation delay
- Hot reloading support
- Interactive REPL
- Immediate feedback

### Tier 2: JIT Compiler (Hot Paths)
- Profile-guided optimization
- Compile frequently-executed code
- Adaptive optimization based on runtime data
- Deoptimization when assumptions break

### Tier 3: AOT Compiler (Production)
- Full ahead-of-time compilation
- Maximum performance
- Link-time optimization (LTO)
- Dead code elimination

## Memory Management

### Garbage Collection
- **Generational GC** for most objects
- **Young generation**: Fast, frequent collection
- **Old generation**: Infrequent, thorough collection
- **Concurrent marking** to reduce pause times

### Optional Manual Control
```sapphire
# Automatic (default)
let data = allocate_large_buffer()

# Manual control for performance-critical code
unsafe:
    let ptr = malloc(1024)
    defer free(ptr)
```

### Reference Counting
- Used for deterministic cleanup (files, sockets)
- Cycle detection to prevent leaks
- Weak references available

## Type System

### Static + Dynamic
```sapphire
# Static typing (checked at compile time)
let x: int = 42

# Type inference
let y = 42  # inferred as int

# Dynamic typing (when needed)
let z: dyn = get_dynamic_value()
```

### Null Safety
```sapphire
# Non-nullable by default
let name: str = "Alice"  # Cannot be null

# Nullable types explicit
let optional_name: str? = None
if optional_name:
    print(optional_name)  # Smart cast to str
```

## Concurrency Model

### Lightweight Threads (Goroutines)
- M:N threading (many user threads on few OS threads)
- Work-stealing scheduler
- Minimal stack overhead (starts at 2KB)

### Async/Await
- Cooperative multitasking
- Zero-cost futures
- Structured concurrency

### Channels
- Type-safe message passing
- Buffered and unbuffered
- Select statement for multiple channels

## Interoperability

### C/C++ FFI
```sapphire
extern "C":
    fn printf(format: *const char, ...) -> int

fn main():
    printf("Hello from C!\n")
```

### Python Integration
```sapphire
import python:
    import numpy as np
    
fn use_numpy():
    let arr = np.array([1, 2, 3])
    return arr.sum()
```

## Standard Library Structure

```
std/
├── collections/    # List, Dict, Set, Queue, etc.
├── io/            # File, network, streams
├── async/         # Async runtime, futures
├── sync/          # Mutexes, channels, atomics
├── math/          # Mathematical functions
├── string/        # String manipulation
├── time/          # Date, time, duration
├── json/          # JSON parsing/serialization
├── http/          # HTTP client/server
├── test/          # Testing framework
└── reflect/       # Reflection and metaprogramming
```

## Error Handling Philosophy

1. **Compile-time errors**: Catch as much as possible early
2. **Result types**: For recoverable errors
3. **Panics**: For unrecoverable errors (with stack traces)
4. **Exceptions**: Optional, for compatibility with other languages

## Performance Goals

- **Startup time**: < 10ms for small programs
- **Compilation**: < 1s per 10,000 lines (debug mode)
- **Runtime**: Within 10% of C++ for compute-intensive tasks
- **Memory**: Comparable to Java/Go with GC
- **Binary size**: Smaller than Go, larger than Rust
