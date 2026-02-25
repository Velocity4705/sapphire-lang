# Milestone 3 Kickoff: Code Generation

## 🎯 Mission

Transform Sapphire from an interpreter into a **compiled language** that generates native machine code, taking the first major step toward becoming the **second-fastest programming language**.

## 📊 Current Status

**Completed:**
- ✅ Milestone 1: Lexer, Parser, Interpreter
- ✅ Milestone 2: Type System with Inference, Generics, Traits

**Starting:**
- 🚀 Milestone 3: LLVM Code Generation

## 🏗️ What We're Building

### Phase 1: LLVM Integration (Weeks 1-2)
**Goal:** Set up LLVM infrastructure

**Deliverables:**
- LLVM code generator class structure
- Basic IR generation for simple expressions
- Type mapping (Sapphire → LLVM types)

**Files Created:**
- ✅ `src/codegen/llvm_codegen.h` - Code generator interface
- ✅ `src/codegen/llvm_codegen.cpp` - Implementation
- ✅ `docs/LLVM_SETUP.md` - Installation guide
- ✅ `docs/MILESTONE3_PLAN.md` - Detailed plan

### Remaining Phases (Weeks 3-12)
- **Phase 2:** Expression code generation
- **Phase 3:** Statement code generation
- **Phase 4:** Function and module system
- **Phase 5:** Optimization and linking
- **Phase 6:** Standard library core

## 🔧 Prerequisites

### Install LLVM

**Fedora (your system):**
```bash
sudo dnf install llvm-devel clang-devel cmake
```

**Verify:**
```bash
llvm-config --version  # Should show 14.0 or higher
```

**Full guide:** See [LLVM_SETUP.md](LLVM_SETUP.md)

## 📁 New Directory Structure

```
src/
├── codegen/              # NEW: Code generation
│   ├── llvm_codegen.h   # ✅ Created
│   ├── llvm_codegen.cpp # ✅ Created
│   ├── module.h         # TODO
│   ├── function.h       # TODO
│   ├── optimizer.h      # TODO
│   └── linker.h         # TODO
├── lexer/               # Existing
├── parser/              # Existing
├── semantic/            # Existing
├── types/               # Existing
└── main.cpp             # Will be updated
```

## 🎯 Success Criteria

### Must Have (End of Milestone 3)
- [ ] Compile simple programs to native executables
- [ ] Executables run correctly
- [ ] Performance within 2x of C
- [ ] Basic optimization works
- [ ] Can link with C libraries

### Stretch Goals
- [ ] Performance within 1.5x of C
- [ ] Multiple optimization levels (-O0, -O1, -O2, -O3)
- [ ] Debug information in binaries
- [ ] Reasonable compilation speed

## 📈 Performance Targets

### Initial Target (End of Phase 5)
```
Benchmark: Fibonacci(40)
C:        0.80s  (baseline)
Sapphire: 1.60s  (2x slower) ← Initial target
C++:      0.82s
Rust:     0.83s
```

### Optimized Target (After optimization work)
```
Benchmark: Fibonacci(40)
C:        0.80s  (baseline)
Sapphire: 0.96s  (1.2x slower) ← Optimized target
C++:      0.82s
Rust:     0.83s
```

### Ultimate Goal (Milestone 8+)
```
Benchmark: Fibonacci(40)
C:        0.80s  (baseline)
Sapphire: 0.81s  (1% slower) ← Ultimate goal
C++:      0.82s
Rust:     0.83s
```

## 🚀 Getting Started

### Step 1: Install LLVM
```bash
sudo dnf install llvm-devel clang-devel cmake
llvm-config --version
```

### Step 2: Update Build System
```bash
# Update CMakeLists.txt to find and link LLVM
# Update Makefile to include codegen directory
```

### Step 3: Test Basic IR Generation
```bash
# Create test program
echo "let x = 10 + 20" > test.spp

# Generate IR (once implemented)
./sapp compile test.spp --emit-llvm -o test.ll

# View IR
cat test.ll
```

### Step 4: Compile to Executable
```bash
# Compile to native code
./sapp compile test.spp -o test

# Run
./test
```

## 📚 Key Concepts

### LLVM IR (Intermediate Representation)
- Platform-independent assembly-like language
- Strongly typed
- SSA (Static Single Assignment) form
- Optimizable

### Example Transformation

**Sapphire Code:**
```sapphire
let x = 10
let y = 20
let sum = x + y
```

**LLVM IR:**
```llvm
define i32 @main() {
entry:
  %x = alloca i64
  store i64 10, i64* %x
  %y = alloca i64
  store i64 20, i64* %y
  %0 = load i64, i64* %x
  %1 = load i64, i64* %y
  %sum = add i64 %0, %1
  ret i32 0
}
```

**Native Assembly (x86_64):**
```asm
main:
  mov rax, 10
  mov rbx, 20
  add rax, rbx
  xor eax, eax
  ret
```

## 🔄 Development Workflow

### 1. Write Code Generator
```cpp
// In llvm_codegen.cpp
llvm::Value* LLVMCodeGen::generateBinaryExpr(BinaryExpr* expr) {
    llvm::Value* left = generateExpr(expr->left.get());
    llvm::Value* right = generateExpr(expr->right.get());
    return builder_->CreateAdd(left, right, "addtmp");
}
```

### 2. Test with Simple Program
```sapphire
let result = 10 + 20
print(result)
```

### 3. Verify IR
```bash
./sapp compile test.spp --emit-llvm
cat test.ll  # Check generated IR
```

### 4. Compile and Run
```bash
./sapp compile test.spp -o test
./test  # Should print 30
```

### 5. Benchmark
```bash
time ./test
# Compare with C++ equivalent
```

## 📖 Resources

### Documentation
- [MILESTONE3_PLAN.md](MILESTONE3_PLAN.md) - Detailed implementation plan
- [LLVM_SETUP.md](LLVM_SETUP.md) - Installation guide
- [PERFORMANCE_STRATEGY.md](PERFORMANCE_STRATEGY.md) - Performance goals

### External Resources
- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
- [Kaleidoscope](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)

## 🎯 Next Actions

### Immediate (This Session)
1. ✅ Create code generator structure
2. ✅ Write LLVM setup guide
3. ✅ Document milestone plan
4. ⏳ Install LLVM on development machine
5. ⏳ Update build system (CMakeLists.txt, Makefile)
6. ⏳ Implement basic IR generation
7. ⏳ Test with simple program

### This Week
- Complete Phase 1 (LLVM Integration)
- Generate IR for arithmetic expressions
- Test with example programs

### This Month
- Complete Phases 2-3 (Expressions and Statements)
- Generate IR for control flow
- Compile first executable

### This Quarter
- Complete all 6 phases
- Achieve 2x C performance
- Begin optimization work

## 🎉 Milestone 3 Vision

By the end of Milestone 3, Sapphire will:

✨ **Compile to native machine code**
- No more interpreter
- Standalone executables
- Platform-native performance

⚡ **Be significantly faster**
- 10-100x faster than Python
- Within 2x of C initially
- Foundation for reaching 0-5% of C

🚀 **Enable real-world use**
- Systems programming
- Performance-critical applications
- Production deployments

🎯 **Prove the vision**
- Second-fastest language is achievable
- LLVM provides the foundation
- Path to ultimate performance is clear

---

**Let's build the second-fastest programming language!** 🚀💎

**Next Step:** Install LLVM and update the build system.
