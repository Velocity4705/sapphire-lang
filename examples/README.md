# Sapphire Examples

This directory contains example programs demonstrating Sapphire's features.

## ✅ Working Examples (Current v1.0.0)

These examples work with the current parser and have been verified:

### `hello.spp` - Hello World
```bash
./sapp examples/hello.spp
```
Output:
```
Hello, Sapphire!
Hello, World!
```

### `simple.spp` - Basic variables and arithmetic
```bash
./sapp examples/simple.spp
```
Output:
```
10
30
```

### `showcase.spp` - Working language features
```bash
./sapp examples/showcase.spp
```
Output:
```
30
200
256.000000
true
true
true
50
78.539750
```

### `data_science_simple.spp` - Basic data processing
```bash
./sapp examples/data_science_simple.spp
```
Output:
```
=== Sapphire Data Science Demo ===

Mean:
10.000000
Sum:
30
Average:
10.000000
Variance:
16.666667
Value above threshold:
true

✓ Data analysis complete!
```

### `test_basic.spp` - Basic operations
```bash
./sapp examples/test_basic.spp
```
Output:
```
52
15
5.000000
true
true
420
4.200000
```

### `test_arithmetic.spp` - Arithmetic operations
```bash
./sapp examples/test_arithmetic.spp
```
Output:
```
15
5
50
2.000000
```

### `test_comparison.spp` - Comparison operators
```bash
./sapp examples/test_comparison.spp
```
Output:
```
true
false
false
true
```

### `file_io_example.spp` - File I/O simulation
```bash
./sapp examples/file_io_example.spp
```
Demonstrates file operations concepts (reading, writing, CSV, JSON).

### `user_input_example.spp` - User input simulation
```bash
./sapp examples/user_input_example.spp
```
Demonstrates user input validation and processing.

### `system_programming_example.spp` - System programming simulation
```bash
./sapp examples/system_programming_example.spp
```
Demonstrates system-level operations (processes, memory, CPU, disk).

### `directives_example.spp` - Directives simulation
```bash
./sapp examples/directives_example.spp
```
Demonstrates compiler directives and optimization concepts.

### Test All Examples
```bash
./test_examples.sh
```

All 11 examples pass! ✅

## 🔮 Future Vision Examples

These examples demonstrate planned features for future versions. They will not run yet but show the direction of the language:

- **`data_science.spp`** - ML and data analysis (requires: imports, classes, async)
- **`advanced_features.spp`** - Advanced language features
- **`advanced_types.spp`** - Dependent types, higher-kinded types
- **`macros.spp`** - Macro system
- **`jit_compilation.spp`** - JIT compilation features
- **`mobile_example.spp`** - Mobile platform directives
- **`platform_directives.spp`** - Platform-specific code
- **`cli_tool.spp`** - CLI application (requires: imports, modules)
- **`complete_cli_tool.spp`** - Full CLI tool
- **`concurrent_example.spp`** - Concurrency (requires: Thread API)
- **`json_example.spp`** - JSON processing (requires: imports)
- **`web_server.spp`** - HTTP server (requires: imports)
- **`microservice.spp`** - Microservice example
- **`systems_programming.spp`** - Systems programming

These are documentation examples showing the vision for Sapphire's future capabilities.

## Current Parser Limitations

The current parser (v1.0.0) supports:

✅ Variables (`let x = 10`)  
✅ Arithmetic (`+`, `-`, `*`, `/`)  
✅ Comparisons (`>`, `<`, `==`, `!=`)  
✅ Print statements  
✅ Comments (`#`)  
✅ Basic expressions  

## Coming Soon

Features being implemented for v1.0.0:

### Control Flow
- 🔄 if/else statements
- 🔄 while loops
- 🔄 for loops
- 🔄 match/switch statements
- 🔄 Pattern matching
- 🔄 Guard clauses

### Functions & Methods
- 🔄 Function definitions
- 🔄 Methods
- 🔄 Lambda expressions
- 🔄 Closures
- 🔄 Higher-order functions
- 🔄 Default parameters
- 🔄 Named parameters
- 🔄 Variadic functions

### Object-Oriented
- 🔄 Classes and objects
- 🔄 Inheritance
- 🔄 Interfaces/Traits
- 🔄 Abstract classes
- 🔄 Properties
- 🔄 Constructors/Destructors
- 🔄 Operator overloading
- 🔄 Method overloading

### Data Structures
- 🔄 Arrays
- 🔄 Lists/Vectors
- 🔄 Hash maps/Dictionaries
- 🔄 Sets
- 🔄 Tuples
- 🔄 Structs
- 🔄 Enums

### Advanced Features
- 🔄 Generics
- 🔄 Type inference
- 🔄 Pattern matching
- 🔄 String interpolation
- 🔄 Imports and modules
- 🔄 Async/await
- 🔄 Coroutines
- 🔄 Macros
- 🔄 Reflection
- 🔄 Decorators

### Memory Management
- 🔄 Ownership system
- 🔄 Borrowing
- 🔄 Lifetimes
- 🔄 Smart pointers
- 🔄 Reference counting
- 🔄 Garbage collection

### Concurrency
- 🔄 Threads
- 🔄 Channels
- 🔄 Mutexes
- 🔄 Async/await
- 🔄 Thread pools
- 🔄 Atomic operations

### Standard Library
- 🔄 Collections (Vec, HashMap, Set)
- 🔄 String manipulation
- 🔄 File I/O
- 🔄 Networking (HTTP, TCP, UDP)
- 🔄 JSON/XML parsing
- 🔄 Regular expressions
- 🔄 Date/Time
- 🔄 Math functions
- 🔄 Cryptography
- 🔄 Compression

### Platform Features
- 🔄 Platform directives (#[ios], #[android], #[web])
- 🔄 Native API access
- 🔄 Cross-platform GUI
- 🔄 Mobile support
- 🔄 WASM compilation

### Tooling
- 🔄 Package manager (spm)
- 🔄 Build system
- 🔄 Test framework
- 🔄 Code formatter
- 🔄 Linter
- 🔄 Debugger
- 🔄 Profiler
- 🔄 Documentation generator
- 🔄 REPL
- 🔄 Language server

**See [COMPLETE_FEATURES.md](../docs/COMPLETE_FEATURES.md) for the full list of 500+ features from all major programming languages.**

## Creating Your Own Examples

For now, stick to the supported features:

```sapphire
# This works!
let x = 10
let y = 20
let sum = x + y
print(sum)

let is_greater = x > 5
print(is_greater)
```

```sapphire
# This doesn't work yet
if x > 5:
    print("Greater")  # Parser doesn't support if/else yet
```

## Testing Examples

Test a specific example:
```bash
./sapp examples/hello.spp
```

Test all working examples:
```bash
for file in examples/hello.spp examples/simple.spp examples/showcase.spp; do
    echo "Testing: $file"
    ./sapp "$file"
    echo ""
done
```

## Contributing Examples

When adding new examples:

1. Test with current parser: `./sapp your_example.spp`
2. If it uses future features, add a note at the top
3. Consider creating a simplified version that works now
4. Update this README

## Documentation

See the main documentation for more details:
- [Getting Started](../docs/GETTING_STARTED.md)
- [Language Features](../docs/LANGUAGE_FEATURES.md)
- [Standard Library](../docs/STANDARD_LIBRARY.md)
