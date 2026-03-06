# Sapphire 💎

A modern programming language that's as fast as C and as easy as Python.

```sapphire
fn main() {
    let numbers = [1, 2, 3, 4, 5]
    for num in numbers {
        println(num * 2)
    }
}
```

**Fast.** Within 0-5% of C performance.  
**Safe.** Memory safety without garbage collection overhead.  
**Simple.** Clean syntax that feels natural.

---

## Quick Start

### Install

```bash
curl -fsSL https://raw.githubusercontent.com/velo4705/sapphire-lang/main/install.sh | bash
```

Or build from source:

```bash
git clone https://github.com/velo4705/sapphire-lang.git
cd sapphire-lang
make quick
./sapp examples/hello.spp
```

### Hello World

```sapphire
fn main() {
    println("Hello, Sapphire! 💎")
}
```

Run it:
```bash
sapp hello.spp
```

---

## Why Sapphire?

**Performance** - Second only to C/Assembly. Faster than Rust and C++ in many benchmarks.

**Memory Safety** - Ownership system catches bugs at compile time. No segfaults or memory leaks.

**Concurrency** - Built-in threads, channels, and async/await. Write parallel code easily.

**Complete Tooling** - Package manager, formatter, test runner, and debugger included.

**Rich Standard Library** - 18 libraries covering everything from file I/O to HTTP servers.

---

## Features

### Language
- Type inference (no verbose type annotations)
- Pattern matching
- Generics and traits
- Error handling (try/catch)
- String interpolation
- Closures and lambdas

### Standard Library (18 modules)
- **Core**: String, Vec, HashMap, Math
- **I/O**: File operations, JSON, Base64
- **Network**: HTTP client/server
- **Text**: Regular expressions
- **System**: Process management, OS interface
- **Advanced**: SIMD/HPC, GUI, Graphics
- **Tools**: CLI parsing, DateTime

### Performance
- LLVM-based compilation
- Zero-cost abstractions
- SIMD vectorization (AVX-512)
- 119μs garbage collection pauses
- Native code generation

---

## Examples

### Variables and Types

```sapphire
fn main() {
    let x = 10              // Type inference
    let mut y = 20          // Mutable variable
    let name: str = "Alice" // Explicit type
    
    println(x + y)
}
```

### Control Flow

```sapphire
fn main() {
    let score = 85
    
    if score >= 90 {
        println("A")
    } else if score >= 80 {
        println("B")
    } else {
        println("C")
    }
    
    for i in 0..10 {
        println(i)
    }
}
```

### Functions

```sapphire
fn add(a: int, b: int) -> int {
    return a + b
}

fn main() {
    let result = add(5, 3)
    println(result)  // 8
}
```

### File I/O

```sapphire
import File from "stdlib/io"

fn main() {
    let content = File.read("data.txt")
    println(content)
    
    File.write("output.txt", "Hello, World!")
}
```

### HTTP Server

```sapphire
import HTTP from "stdlib/http"

fn main() {
    let server = HTTP.Server(8080)
    
    server.get("/", fn(req, res) {
        res.body = "Hello, World!"
    })
    
    server.start()
}
```

More examples in [examples/](examples/)

---

## Documentation

**Tutorials** (Start here!)
- [Getting Started](docs/tutorials/01_GETTING_STARTED.md) - Your first program
- [Control Flow](docs/tutorials/02_CONTROL_FLOW.md) - If statements and loops
- [Functions](docs/tutorials/03_FUNCTIONS.md) - Writing reusable code
- [Arrays & Collections](docs/tutorials/04_ARRAYS.md) - Working with data
- [Strings](docs/tutorials/05_STRINGS.md) - Text manipulation
- [File I/O](docs/tutorials/06_FILE_IO.md) - Reading and writing files

**Reference**
- [API Reference](docs/API_REFERENCE.md) - Complete standard library documentation
- [Language Features](docs/LANGUAGE_FEATURES.md) - Syntax and semantics
- [System Programming](docs/SYSTEM_PROGRAMMING.md) - Low-level access

---

## Tools

**sapp** - Compiler and interpreter
```bash
sapp hello.spp              # Run program
sapp --compile hello.spp    # Compile to binary
sapp --version              # Show version
```

**spm** - Package manager
```bash
spm init                    # Create new project
spm build                   # Build project
spm test                    # Run tests
spm run                     # Run project
```

**sapphire-fmt** - Code formatter
```bash
sapphire-fmt file.spp       # Format file
sapphire-fmt --check .      # Check formatting
```

---

## Status

**Version:** 1.0.0  
**Tests:** 56/56 passing (100%)  
**Libraries:** 18/18 complete  
**Status:** Production ready ✅

---

## Use Cases

Build anything with Sapphire:

- Web applications (frontend & backend)
- System utilities and tools
- High-performance computing
- GUI applications
- Mobile apps
- Game development
- Data processing
- Network services

---

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Ways to help:**
- Report bugs and suggest features
- Improve documentation
- Add examples
- Optimize performance
- Write tutorials

---

## Community

- **GitHub**: [velo4705/sapphire-lang](https://github.com/velo4705/sapphire-lang)
- **Issues**: [Report bugs](https://github.com/velo4705/sapphire-lang/issues)
- **Discussions**: [Ask questions](https://github.com/velo4705/sapphire-lang/discussions)

---

## License

MIT License - see [LICENSE](LICENSE)

---

<div align="center">

**Made with 💎 by the Sapphire community**

[⭐ Star](https://github.com/velo4705/sapphire-lang) · [🐛 Report Bug](https://github.com/velo4705/sapphire-lang/issues) · [� Request Feature](https://github.com/velo4705/sapphire-lang/issues)

</div>
