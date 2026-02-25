# Milestone 3: Code Generation - Implementation Plan

## Goal
Transform Sapphire from an interpreter into a true compiled language that generates native machine code via LLVM, achieving our goal of being the second-fastest programming language.

## Timeline
Months 7-9 (3 months)

## Phases

### Phase 1: LLVM Integration (Week 1-2)
**Objective:** Set up LLVM infrastructure and basic IR generation

#### Tasks
1. Install LLVM development libraries
2. Create LLVM code generator class structure
3. Set up LLVM module, context, and builder
4. Generate IR for simple expressions (literals, arithmetic)
5. Test basic IR generation

#### Deliverables
- `src/codegen/llvm_codegen.h` - Code generator interface
- `src/codegen/llvm_codegen.cpp` - LLVM IR generation
- `src/codegen/module.h` - Module management
- Basic IR output for simple programs

#### Success Criteria
- Can generate LLVM IR for: `let x = 10 + 20`
- IR is valid and can be printed
- No crashes or memory leaks

---

### Phase 2: Expression Code Generation (Week 3-4)
**Objective:** Generate IR for all expression types

#### Tasks
1. Binary operations (arithmetic, logical, comparison)
2. Unary operations (negation, not)
3. Variable references and assignments
4. Function calls
5. Type conversions
6. Array/list operations

#### Deliverables
- Complete expression code generation
- Type-aware IR generation
- Optimization hints for LLVM

#### Success Criteria
- All expression types generate valid IR
- Type information preserved
- Can compile: `let result = (x + y) * 2 > 10`

---

### Phase 3: Statement Code Generation (Week 5-6)
**Objective:** Generate IR for control flow and statements

#### Tasks
1. Variable declarations
2. If/else statements (branching)
3. While loops
4. For loops (with ranges)
5. Return statements
6. Function definitions
7. Block scoping

#### Deliverables
- Complete statement code generation
- Control flow graph generation
- Basic block management

#### Success Criteria
- Can compile loops and conditionals
- Function definitions work
- Proper scoping in generated code

---

### Phase 4: Function and Module System (Week 7-8)
**Objective:** Generate complete modules with functions

#### Tasks
1. Function signature generation
2. Function body generation
3. Parameter passing
4. Return value handling
5. Module initialization
6. Entry point (main function)
7. External function declarations

#### Deliverables
- Complete function code generation
- Module linking support
- Entry point generation

#### Success Criteria
- Can compile multi-function programs
- Functions can call each other
- Main function executes correctly

---

### Phase 5: Optimization and Linking (Week 9-10)
**Objective:** Optimize IR and link to executable

#### Tasks
1. LLVM optimization passes
   - Dead code elimination
   - Constant folding
   - Inline functions
   - Loop optimizations
2. Object file generation
3. Linking with system libraries
4. Executable generation
5. Optimization levels (-O0, -O1, -O2, -O3)

#### Deliverables
- Optimization pass manager
- Linker integration
- Executable output
- Build system updates

#### Success Criteria
- Programs compile to native executables
- Executables run correctly
- Optimization improves performance
- Binary size is reasonable

---

### Phase 6: Standard Library Core (Week 11-12)
**Objective:** Implement essential stdlib functions

#### Tasks
1. Print function (native implementation)
2. String operations
3. Array/list operations
4. Math functions
5. I/O basics (file read/write)
6. Memory allocation helpers

#### Deliverables
- `stdlib/core.spp` - Core functions
- `runtime/runtime.cpp` - Runtime support
- FFI bindings for C stdlib

#### Success Criteria
- Print works in compiled code
- Basic I/O functions work
- Can use stdlib in programs

---

## Technical Architecture

### Code Generator Structure
```
src/codegen/
├── llvm_codegen.h       # Main code generator
├── llvm_codegen.cpp     # IR generation implementation
├── module.h             # Module management
├── module.cpp
├── function.h           # Function code generation
├── function.cpp
├── expression.h         # Expression code generation
├── expression.cpp
├── statement.h          # Statement code generation
├── statement.cpp
├── optimizer.h          # Optimization passes
├── optimizer.cpp
├── linker.h             # Linking and executable generation
└── linker.cpp
```

### LLVM Integration Points
```cpp
// Core LLVM components
llvm::LLVMContext context;
llvm::Module* module;
llvm::IRBuilder<> builder;
llvm::FunctionPassManager* fpm;

// Type mapping
Sapphire Type -> LLVM Type
int           -> i64
float         -> double
string        -> i8*
bool          -> i1
List<T>       -> struct { i64 size, T* data }
```

### Compilation Pipeline
```
Source Code (.spp)
    ↓
Lexer (tokens)
    ↓
Parser (AST)
    ↓
Type Checker (typed AST)
    ↓
LLVM Code Generator (LLVM IR)
    ↓
LLVM Optimizer (optimized IR)
    ↓
LLVM Backend (object file)
    ↓
Linker (executable)
    ↓
Native Binary
```

## Performance Targets

### Compilation Speed
- Small programs (< 100 lines): < 100ms
- Medium programs (< 1000 lines): < 1s
- Large programs (< 10000 lines): < 10s

### Runtime Performance (vs C baseline)
- Arithmetic operations: 100% (same as C)
- Function calls: 100% (inlined)
- Loops: 100% (optimized)
- Memory allocation: 95% (slight overhead)
- Overall: 95-100% of C performance

### Binary Size
- Hello World: < 50KB
- Medium program: < 500KB
- With stdlib: < 2MB

## Build System Updates

### New Makefile Targets
```makefile
# Compile Sapphire program to executable
compile:
	./sapp compile input.spp -o output

# Compile with optimization
compile-opt:
	./sapp compile input.spp -o output -O3

# Show LLVM IR
show-ir:
	./sapp compile input.spp --emit-llvm

# Show assembly
show-asm:
	./sapp compile input.spp --emit-asm
```

### CMake Updates
```cmake
# Find LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Link LLVM libraries
target_link_libraries(sapp ${LLVM_LIBS})
```

## Testing Strategy

### Unit Tests
- IR generation for each AST node type
- Type mapping correctness
- Optimization pass validation

### Integration Tests
- Compile and run example programs
- Compare output with interpreter
- Performance benchmarks

### Benchmark Programs
1. Fibonacci (recursive and iterative)
2. Prime number sieve
3. Matrix multiplication
4. String manipulation
5. File I/O operations

### Performance Comparison
```bash
# Run benchmarks
./scripts/benchmark.sh

# Compare with other languages
./scripts/compare.sh --languages=c,cpp,rust,go,python
```

## Dependencies

### Required Libraries
- LLVM 14+ (development libraries)
- Clang (for linking)
- CMake 3.15+
- C++20 compiler

### Installation (Ubuntu/Debian)
```bash
sudo apt-get install llvm-14-dev clang-14 cmake
```

### Installation (macOS)
```bash
brew install llvm@14 cmake
```

## Success Criteria

### Must Have
✅ Compile simple programs to native executables
✅ Executables run correctly
✅ Performance within 2x of C
✅ Basic optimization works
✅ Can link with C libraries

### Should Have
✅ Performance within 1.5x of C
✅ Multiple optimization levels
✅ Reasonable compilation speed
✅ Good error messages during compilation

### Nice to Have
✅ Performance within 1.1x of C (approach our 0-5% goal)
✅ Debug information in binaries
✅ Cross-compilation support
✅ Incremental compilation

## Risks and Mitigations

### Risk 1: LLVM Complexity
**Mitigation:** Start with simple IR generation, use LLVM tutorials and examples

### Risk 2: Performance Not Meeting Goals
**Mitigation:** Profile generated code, enable all LLVM optimizations, add custom passes

### Risk 3: Linking Issues
**Mitigation:** Use system linker (ld/lld), test on multiple platforms early

### Risk 4: Memory Management
**Mitigation:** Use stack allocation by default, defer GC to Milestone 4

## Next Steps After Milestone 3

1. **Milestone 4:** Memory management (GC, ownership)
2. **Milestone 5:** Concurrency (async/await, goroutines)
3. **Milestone 6:** Standard library expansion
4. **Milestone 7:** Developer tooling (LSP, debugger)

## Documentation Updates

- [ ] Update README with compilation instructions
- [ ] Create LLVM code generation guide
- [ ] Document optimization flags
- [ ] Add compilation examples
- [ ] Update performance benchmarks

## Timeline Summary

| Week | Phase | Deliverable |
|------|-------|-------------|
| 1-2  | LLVM Integration | Basic IR generation |
| 3-4  | Expressions | All expression types |
| 5-6  | Statements | Control flow |
| 7-8  | Functions | Multi-function programs |
| 9-10 | Optimization | Native executables |
| 11-12| Standard Library | Core stdlib |

**Total:** 12 weeks (3 months)

## Conclusion

Milestone 3 is the most critical milestone - it transforms Sapphire from a toy interpreter into a real compiled language. By the end, we'll have:

- Native code generation via LLVM
- Performance approaching C (within 2x initially, optimizing toward 0-5%)
- A foundation for becoming the second-fastest language
- The ability to compile real programs to standalone executables

Let's build the second-fastest programming language! 🚀
