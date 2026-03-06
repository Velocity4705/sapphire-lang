# Quick Start Guide

## Installation

### Build from Source

```bash
# Clone repository
git clone https://github.com/velo4705/sapphire-lang.git
cd sapphire-lang

# Build (requires C++20 compiler)
make quick

# Verify installation
./sapphire --version
```

## Your First Program

Create `hello.spp`:

```sapphire
# Hello World in Sapphire
let message = "Hello, Sapphire!"
print(message)
```

Run it:

```bash
./sapp hello.spp
# or
./sapphire hello.spp
```

**Note:** Both `sapp` (short) and `sapphire` (full) work!

Output:
```
Hello, Sapphire!
```

## Basic Examples

### Variables and Arithmetic

```sapphire
let x = 10
let y = 20
let sum = x + y
print(sum)  # 30

let power = 2 ** 8
print(power)  # 256
```

### Conditionals

```sapphire
let age = 25

if age >= 18:
    print("Adult")
else:
    print("Minor")
```

### Loops

```sapphire
# While loop
let i = 0
while i < 5:
    print(i)
    i = i + 1

# For loop
for j in range(10):
    print(j)
```

### Comments

```sapphire
# This is a single-line comment

let x = 42  # Inline comment
```

## What Works Now (Milestone 1)

✅ Variables (let/const)  
✅ Integers, floats, strings, booleans  
✅ Arithmetic (+, -, *, /, %, **)  
✅ Comparisons (==, !=, <, <=, >, >=)  
✅ Logical operators (and, or, not)  
✅ If/else statements  
✅ While loops  
✅ For loops with range  
✅ Print function  
✅ Comments  

## What's Coming Next (Milestone 2)

⏳ Type system and inference  
⏳ Function calls  
⏳ Classes and objects  
⏳ Imports  
⏳ Better error messages  
⏳ More built-in functions  

## More Examples

Check out the `examples/` directory:

```bash
./sapphire examples/demo.spp
./sapphire examples/simple.spp
```

## Getting Help

- Documentation: `docs/`
- Examples: `examples/`
- Issues: GitHub Issues
- Contributing: `CONTRIBUTING.md`

## Next Steps

1. Read [Language Features](LANGUAGE_FEATURES.md)
2. Explore [Examples](../examples/)
3. Check [Project Status](PROJECT_STATUS.md)
4. Contribute! See [CONTRIBUTING.md](../CONTRIBUTING.md)
