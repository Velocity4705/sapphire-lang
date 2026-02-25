# Sapphire Command Name

## Why Two Names?

Sapphire provides **two commands** for flexibility:

### `sapp` (Primary - 4 characters)
- **Short and fast to type**
- Stands for "Sapphire App" or "Sapphire Compiler"
- Similar to: `rustc`, `javac`, `clang`
- Recommended for daily use

### `sapphire` (Alias - 8 characters)
- **Full name for clarity**
- Better for documentation and tutorials
- Self-documenting in scripts
- Alias/symlink to `sapp`

## Usage

Both commands work identically:

```bash
# Using short name (recommended)
sapp hello.spp
sapp --version
sapp --help

# Using full name (also works)
sapphire hello.spp
sapphire --version
sapphire --help
```

## Comparison with Other Languages

| Language | Short | Full | Notes |
|----------|-------|------|-------|
| Python | `py` | `python` | Both widely used |
| TypeScript | `tsc` | - | TypeScript Compiler |
| Rust | `rustc` | - | Rust Compiler |
| Go | `go` | - | Very short |
| Java | `javac` | - | Java Compiler |
| C/C++ | `gcc`, `clang` | - | Short names |
| **Sapphire** | **`sapp`** | **`sapphire`** | Best of both! |

## Installation

When installed system-wide, both commands are available:

```bash
# Install
sudo make install

# Both work
sapp myprogram.spp
sapphire myprogram.spp
```

## In Scripts

For scripts and documentation, you can use either:

```bash
#!/usr/bin/env sapp
# Short and clear

#!/usr/bin/env sapphire  
# Self-documenting
```

## Recommendation

- **Daily use**: Use `sapp` (faster to type)
- **Documentation**: Use `sapphire` (clearer)
- **Scripts**: Either works, choose based on preference

The choice is yours! 💎
