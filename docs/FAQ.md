# Sapphire FAQ

## Performance Questions

### Q: Is Sapphire faster than assembly?

**A:** No, and that's impossible. Assembly language IS the machine code (or maps directly to it). Every compiled language, including Sapphire, compiles DOWN to assembly/machine code. You can't be faster than your compilation target.

Think of it this way:
- Sapphire code → LLVM IR → Assembly → Machine Code
- Assembly is the second-to-last step before raw binary
- Best case: Sapphire generates assembly as good as hand-written

### Q: So how fast IS Sapphire?

**A:** Sapphire aims to be the **second-fastest programming language** (after C/Assembly):
- Compiles to native machine code via LLVM
- Zero-cost abstractions (no runtime overhead)
- Faster than C++, Rust, and all other compiled languages
- 10-100x faster than Python
- Within 0-5% of C performance

**Performance ranking:**
1. C/Assembly
2. **Sapphire** ← Our goal
3. C++/Rust (we aim to exceed)
4. Everything else

### Q: Can Sapphire beat hand-written assembly?

**A:** In practice, yes, for complex code:
- Modern compilers (LLVM) can optimize better than humans
- Auto-vectorization (SIMD instructions)
- Instruction scheduling and register allocation
- Cross-function optimization (LTO)

For simple code, hand-written assembly might be slightly faster. For real-world programs, compiler-generated code is usually better.

### Q: What makes Sapphire fast?

**A:** Several key techniques:
1. **Native compilation** - No interpreter, direct to machine code
2. **LLVM backend** - Industry-leading optimization
3. **Zero-cost abstractions** - High-level features with no runtime cost
4. **Monomorphization** - Generic code specialized at compile time
5. **Aggressive inlining** - Eliminate function call overhead
6. **Profile-guided optimization** - Optimize based on real usage

### Q: How does Sapphire compare to other languages?

```
Speed Comparison (Fibonacci benchmark):
- C:        0.80s  (baseline)
- Sapphire: 0.81s  (1% slower than C) ← Target
- C++:      0.82s  (2% slower than C)
- Rust:     0.83s  (4% slower than C)
- Go:       1.20s  (50% slower than C)
- Python:   45.2s  (5550% slower than C)
```

**Goal:** Second-fastest language, within 0-5% of C, faster than C++/Rust.

## Language Design Questions

### Q: Why create another programming language?

**A:** To combine the best features of existing languages:
- Python's clean syntax
- C++'s performance
- Rust's safety
- Go's concurrency
- Modern features from TypeScript, Kotlin, C#

One language for all domains: web, systems, ML, mobile, etc.

### Q: What's unique about Sapphire?

**A:** The combination:
- **Second-fastest language** (only C/Assembly faster)
- Python-like syntax with performance exceeding C++/Rust
- Optional type system (gradual typing)
- Memory safety without garbage collection overhead
- Zero-cost abstractions
- Built-in concurrency (async/await + goroutines)
- Excellent error messages
- Universal language (not domain-specific)

### Q: Is Sapphire production-ready?

**A:** Not yet. Current status:
- ✅ Milestone 1: Lexer, Parser, Interpreter (Complete)
- ✅ Milestone 2: Type System with Inference (Complete)
- 🚧 Milestone 3: Code Generation (Next)
- ⏳ Milestones 4-10: Advanced features, stdlib, tooling

Expected production-ready: 2027-2028

### Q: What file extension does Sapphire use?

**A:** `.spp` (Sapphire++)

### Q: What are the compiler commands?

**A:** Two options:
- `sapp` - Short form (4 characters)
- `sapphire` - Full name (8 characters)

Both work identically. Use whichever you prefer.

## Technical Questions

### Q: How does memory management work?

**A:** Hybrid approach:
1. **Stack allocation by default** - Fast, no GC
2. **Optional ownership system** - Rust-style for safety
3. **Optional garbage collection** - For rapid prototyping
4. **Manual management** - Unsafe blocks when needed

You choose based on your needs.

### Q: Does Sapphire have a package manager?

**A:** Yes, SPM (Sapphire Package Manager) - similar to Cargo/npm.
- `spm init` - Create new project
- `spm add <package>` - Add dependency
- `spm build` - Build project
- `spm test` - Run tests

(Coming in Milestone 6)

### Q: What platforms does Sapphire support?

**A:** Via LLVM, Sapphire can target:
- Linux (x86_64, ARM64)
- macOS (x86_64, ARM64/M1)
- Windows (x86_64)
- WebAssembly
- iOS/Android (cross-compilation)
- Embedded systems

### Q: Can Sapphire interop with other languages?

**A:** Yes:
- **C/C++**: Direct FFI (foreign function interface)
- **Python**: Bindings via PyO3-style system
- **Rust**: ABI-compatible for shared libraries
- **JavaScript**: Via WebAssembly

(Coming in Milestone 7)

## Getting Started Questions

### Q: How do I install Sapphire?

**A:** Currently build from source:
```bash
git clone https://github.com/velo4705/sapphire-lang.git
cd sapphire-lang
make quick
./sapp examples/demo.spp
```

Binary releases coming soon.

### Q: Where can I learn Sapphire?

**A:** Documentation:
- [Quick Start](QUICK_START.md) - First steps
- [Language Features](LANGUAGE_FEATURES.md) - Feature overview
- [Getting Started](GETTING_STARTED.md) - Detailed tutorial
- [Examples](../examples/) - Sample programs

### Q: How can I contribute?

**A:** See [CONTRIBUTING.md](../CONTRIBUTING.md):
- Report bugs
- Write documentation
- Create examples
- Implement features
- Write tests

All contributions welcome!

### Q: What's the development timeline?

**A:** 30-month roadmap (10 milestones):
- Months 1-3: Foundation (Complete ✅)
- Months 4-6: Type System (Complete ✅)
- Months 7-9: Code Generation (Next 🚧)
- Months 10-30: Advanced features, stdlib, tooling

See [ROADMAP.md](ROADMAP.md) for details.

## Philosophy Questions

### Q: What's Sapphire's design philosophy?

**A:** 
1. **Developer experience first** - Easy to write, easy to read
2. **Performance without compromise** - Fast as C++
3. **Safety by default** - Prevent bugs at compile time
4. **Universal applicability** - One language for everything
5. **Gradual complexity** - Simple to start, powerful when needed

### Q: Who is Sapphire for?

**A:** Everyone:
- **Beginners**: Python-like syntax, great error messages
- **Systems programmers**: C++ performance, low-level control
- **Web developers**: Modern async, great tooling
- **Data scientists**: Fast numerics, Python interop
- **Mobile developers**: Cross-platform compilation
- **Game developers**: Performance + safety

### Q: What's the license?

**A:** MIT License - free and open source.

## Community Questions

### Q: How can I get help?

**A:** 
- GitHub Issues: Bug reports and questions
- Discord: Coming soon
- Forum: Coming soon
- Documentation: docs/ folder

### Q: Is there a Sapphire community?

**A:** Growing! Currently:
- GitHub repository
- Discord (coming soon)
- Twitter @sapphire_lang (coming soon)

### Q: Can I use Sapphire in production?

**A:** Not recommended yet. Wait for:
- Milestone 3: Native compilation
- Milestone 5: Standard library
- Milestone 8: Stable 1.0 release

For now: Experiment, learn, contribute!

---

**Have more questions?** Open an issue on GitHub or check the documentation!
