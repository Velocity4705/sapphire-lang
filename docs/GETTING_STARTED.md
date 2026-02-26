# Getting Started with Sapphire

Welcome to Sapphire! This guide will help you get up and running in 15 minutes.

## What is Sapphire?

Sapphire is a modern programming language that combines:
- 🐍 Python's clean syntax
- ⚡ C++'s performance
- 🦀 Rust's safety
- 🔵 Go's concurrency

## Installation

### Prerequisites
- C++20 compiler (g++ or clang++)
- LLVM (optional, for code generation)
- Make

### Build from Source

```bash
git clone https://github.com/Velocity4705/sapphire-lang.git
cd sapphire-lang
make quick
```

This creates the `sapp` compiler.

## Your First Program

Create a file called `hello.spp`:

```sapphire
# Hello World in Sapphire
print("Hello, Sapphire!")
```

Run it:

```bash
./sapp hello.spp
```

Output:
```
Hello, Sapphire!
```

## Using the Package Manager

Sapphire comes with `spm`, a package manager for creating and managing projects.

### Create a New Project

```bash
./spm init my-project
cd my-project
```

This creates:
```
my-project/
  spm.toml          # Project configuration
  src/
    main.spp        # Main source file
  tests/            # Test files
  docs/             # Documentation
```

### Build and Run

```bash
../spm build    # Build the project
../spm run      # Build and run
```

### Run Tests

```bash
../spm test
```

## Language Basics

### Variables

```sapphire
let x = 42
let name = "Alice"
let pi = 3.14159
```

### Functions

```sapphire
fn add(a: int, b: int) -> int {
    return a + b
}

let result = add(10, 20)
print(result)  # 30
```

### Control Flow

```sapphire
# If statements
if x > 10 {
    print("x is big")
} else {
    print("x is small")
}

# While loops
let i = 0
while i < 5 {
    print(i)
    i = i + 1
}
```

### Data Structures

```sapphire
# Arrays (Vec)
let numbers = Vec::new()
numbers.push(1)
numbers.push(2)
numbers.push(3)

# Hash maps
let ages = HashMap::new()
ages["Alice"] = 30
ages["Bob"] = 25

# Strings
let greeting = "Hello, " + "World!"
```

## Standard Library

Sapphire includes a rich standard library:

### String Operations

```sapphire
let text = "Hello, World!"
let upper = text.to_upper()
let parts = text.split(", ")
```

### File I/O

```sapphire
# Write to file
File::write_all("data.txt", "Hello, File!")

# Read from file
let content = File::read_all("data.txt")
print(content)
```

### Math Functions

```sapphire
let x = Math::sqrt(16.0)    # 4.0
let y = Math::sin(Math::PI)  # 0.0
let z = Math::pow(2.0, 3.0)  # 8.0
```

### JSON

```sapphire
# Parse JSON
let json = JSON::parse('{"name": "Alice", "age": 30}')
print(json["name"])  # Alice

# Create JSON
let obj = JSON::object()
obj["name"] = "Bob"
obj["age"] = 25
print(obj.serialize())
```

## Concurrency

Sapphire has built-in concurrency support:

### Threads

```sapphire
# Spawn a thread
let t = Thread::spawn(|| {
    print("Hello from thread!")
})
t.join()
```

### Channels

```sapphire
# Create a channel
let ch = Channel::new()

# Send in one thread
Thread::spawn(|| {
    ch.send(42)
})

# Receive in another
let value = ch.receive()
print(value)  # 42
```

### Thread Pool

```sapphire
# Create a thread pool
let pool = ThreadPool::new(4)

# Submit tasks
let future = pool.submit(|| {
    return expensive_computation()
})

let result = future.get()
```

## Next Steps

- Read the [Language Tutorial](LANGUAGE_TUTORIAL.md) for in-depth coverage
- Explore the [Standard Library Guide](STANDARD_LIBRARY.md)
- Learn about [Concurrency](CONCURRENCY_GUIDE.md)
- Check out [Example Projects](../examples/)

## Getting Help

- Documentation: `docs/` directory
- Examples: `examples/` directory
- Issues: GitHub Issues
- Community: (Coming soon!)

## Quick Reference

### Common Commands

```bash
# Compiler
./sapp program.spp              # Run program
./sapp compile program.spp      # Compile to LLVM IR

# Package Manager
./spm init <name>               # Create new project
./spm build                     # Build project
./spm run                       # Build and run
./spm test                      # Run tests
./spm clean                     # Clean build artifacts
./spm doc                       # Generate docs
./spm fmt                       # Format code

# Tools
./sapphire-fmt file.spp         # Format code
./sapphire-fmt --check file.spp # Check formatting
```

### Syntax Cheat Sheet

```sapphire
# Comments
# This is a comment

# Variables
let x = 42
let name = "Alice"

# Functions
fn add(a: int, b: int) -> int {
    return a + b
}

# Control flow
if condition {
    # ...
} else {
    # ...
}

while condition {
    # ...
}

# Data structures
let vec = Vec::new()
let map = HashMap::new()
let str = String::new()

# Concurrency
let thread = Thread::spawn(|| { })
let channel = Channel::new()
let pool = ThreadPool::new(4)
```

Welcome to Sapphire! Happy coding! 💎
