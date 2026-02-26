# Sapphire Examples

This directory contains example programs demonstrating Sapphire's features.

## ✅ Working Examples (Current v1.0.0)

These examples work with the current parser:

- **`hello.spp`** - Hello World
- **`simple.spp`** - Basic variables and arithmetic
- **`showcase.spp`** - Working language features
- **`data_science_simple.spp`** - Basic data processing
- **`test_basic.spp`** - Basic operations
- **`test_arithmetic.spp`** - Arithmetic operations
- **`test_comparison.spp`** - Comparison operators

### Run a working example:
```bash
./sapp examples/hello.spp
./sapp examples/showcase.spp
./sapp examples/data_science_simple.spp
```

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

Features being implemented:

- 🔄 Control flow (if/else, while, for)
- 🔄 Functions
- 🔄 Classes and objects
- 🔄 Imports and modules
- 🔄 String interpolation
- 🔄 Arrays and collections
- 🔄 Pattern matching

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
