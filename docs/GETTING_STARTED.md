# Getting Started with Sapphire

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/sapphire-lang/sapphire.git
cd sapphire

# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake llvm-14-dev

# Build
./scripts/build.sh

# Install
sudo ./scripts/install.sh
```

### Using Package Manager (Coming Soon)

```bash
# macOS
brew install sapphire

# Linux
curl -sSf https://sapphire-lang.org/install.sh | sh

# Windows
winget install sapphire
```

## Your First Program

Create a file `hello.spp`:

```sapphire
fn main():
    print("Hello, Sapphire!")
```

Run it:

```bash
sapphire run hello.spp
```

Or compile to executable:

```bash
sapphire build hello.spp -o hello
./hello
```

## Interactive REPL

```bash
sapphire repl
```

```sapphire
>>> let x = 42
>>> print(x * 2)
84
>>> fn greet(name: str):
...     print(f"Hello, {name}!")
>>> greet("World")
Hello, World!
```

## Project Structure

Create a new project:

```bash
sapphire new my-project
cd my-project
```

This creates:

```
my-project/
├── spm.toml              # Package configuration
├── src/
│   └── main.spp          # Entry point
├── tests/
│   └── test_main.spp     # Tests
└── README.md
```

## Package Management

Install dependencies:

```bash
spm install http json
```

Add to `spm.toml`:

```toml
[package]
name = "my-project"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]

[dependencies]
http = "1.0"
json = "2.1"
```

## Next Steps

- [Language Syntax](SYNTAX.md)
- [Standard Library](STANDARD_LIBRARY.md)
- [Examples](../examples/)
- [API Reference](API_REFERENCE.md)
