# LLVM Setup Guide for Sapphire

## Overview

Milestone 3 requires LLVM development libraries to compile Sapphire programs to native machine code. This guide covers installation on various platforms.

## Why LLVM?

LLVM is the industry-standard compiler infrastructure used by:
- Clang (C/C++ compiler)
- Rust compiler
- Swift compiler
- Julia language
- Many others

It provides:
- Excellent optimization passes
- Multiple target architectures
- Well-tested code generation
- Active development and support

## Installation

### Fedora / RHEL / CentOS

```bash
# Install LLVM development libraries
sudo dnf install llvm-devel clang-devel cmake

# Verify installation
llvm-config --version
```

### Ubuntu / Debian

```bash
# Install LLVM 14 (or latest available)
sudo apt-get update
sudo apt-get install llvm-14-dev clang-14 cmake

# Create symlinks if needed
sudo ln -s /usr/bin/llvm-config-14 /usr/bin/llvm-config
sudo ln -s /usr/bin/clang-14 /usr/bin/clang

# Verify installation
llvm-config --version
```

### macOS

```bash
# Install via Homebrew
brew install llvm cmake

# Add LLVM to PATH (add to ~/.zshrc or ~/.bash_profile)
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"

# Verify installation
llvm-config --version
```

### Arch Linux

```bash
# Install LLVM
sudo pacman -S llvm clang cmake

# Verify installation
llvm-config --version
```

### From Source (Advanced)

```bash
# Download LLVM source
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout llvmorg-14.0.0

# Build LLVM (takes 30-60 minutes)
mkdir build && cd build
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_ENABLE_PROJECTS="clang" \
    ../llvm

make -j$(nproc)
sudo make install
```

## Verifying Installation

After installation, verify LLVM is available:

```bash
# Check LLVM version
llvm-config --version

# Check LLVM libraries
llvm-config --libs core

# Check include path
llvm-config --includedir

# Check library path
llvm-config --libdir
```

Expected output:
```
14.0.0 (or higher)
-lLLVMCore -lLLVMSupport ...
/usr/include/llvm-14
/usr/lib/llvm-14/lib
```

## Building Sapphire with LLVM

Once LLVM is installed, rebuild Sapphire:

```bash
# Clean previous build
make clean

# Build with LLVM support
make

# Or use CMake directly
mkdir build && cd build
cmake ..
make
```

The build system will automatically detect LLVM and enable code generation.

## Testing LLVM Integration

```bash
# Create a simple test program
cat > test.spp << 'EOF'
let x = 10
let y = 20
let sum = x + y
print(sum)
EOF

# Compile to LLVM IR
./sapp compile test.spp --emit-llvm -o test.ll

# View the generated IR
cat test.ll

# Compile to executable
./sapp compile test.spp -o test

# Run the executable
./test
```

## Troubleshooting

### "llvm-config not found"

**Solution:** LLVM is not installed or not in PATH.

```bash
# Find llvm-config
find /usr -name "llvm-config*" 2>/dev/null

# Add to PATH if found
export PATH="/usr/lib/llvm-14/bin:$PATH"
```

### "Cannot find LLVM libraries"

**Solution:** Install development packages, not just runtime.

```bash
# Ubuntu/Debian
sudo apt-get install llvm-14-dev

# Fedora
sudo dnf install llvm-devel
```

### "LLVM version too old"

**Solution:** Sapphire requires LLVM 14 or newer.

```bash
# Check version
llvm-config --version

# Upgrade if needed
sudo apt-get install llvm-14-dev  # Ubuntu
sudo dnf upgrade llvm-devel       # Fedora
brew upgrade llvm                 # macOS
```

### "Linking errors during build"

**Solution:** Ensure all LLVM components are installed.

```bash
# Ubuntu/Debian
sudo apt-get install llvm-14-dev clang-14 libc++-14-dev

# Fedora
sudo dnf install llvm-devel clang-devel libcxx-devel
```

### "CMake cannot find LLVM"

**Solution:** Set LLVM_DIR environment variable.

```bash
# Find LLVM CMake files
find /usr -name "LLVMConfig.cmake" 2>/dev/null

# Set LLVM_DIR
export LLVM_DIR=/usr/lib/llvm-14/lib/cmake/llvm

# Rebuild
cmake .. -DLLVM_DIR=$LLVM_DIR
make
```

## LLVM Components Used

Sapphire uses the following LLVM components:

- **Core**: Basic IR types and operations
- **Support**: Utilities and data structures
- **IRReader**: Reading LLVM IR
- **Passes**: Optimization passes
- **Target**: Target machine code generation
- **X86**: x86/x64 code generation (or ARM on ARM systems)
- **AsmParser**: Assembly parsing
- **BitWriter**: Bitcode writing

## Performance Notes

### Compilation Speed

LLVM compilation is slower than interpretation but produces much faster code:

- Small programs: 100-500ms compile time
- Medium programs: 1-5s compile time
- Large programs: 10-60s compile time

### Runtime Performance

LLVM-compiled code is significantly faster:

- 10-100x faster than interpreted Python
- Comparable to C++ (within 0-20% initially)
- Goal: Within 0-5% of C (second-fastest language)

### Optimization Levels

```bash
# No optimization (fast compile, slow runtime)
./sapp compile -O0 program.spp

# Basic optimization (balanced)
./sapp compile -O1 program.spp

# Default optimization (recommended)
./sapp compile -O2 program.spp

# Aggressive optimization (slow compile, fast runtime)
./sapp compile -O3 program.spp
```

## Next Steps

After LLVM is installed and working:

1. Read [MILESTONE3_PLAN.md](MILESTONE3_PLAN.md) for implementation details
2. Try compiling example programs
3. Run benchmarks to verify performance
4. Report any issues on GitHub

## Resources

- [LLVM Official Documentation](https://llvm.org/docs/)
- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
- [Kaleidoscope Tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)

## Support

If you encounter issues:

1. Check this troubleshooting guide
2. Search GitHub issues
3. Ask on Discord (coming soon)
4. Open a new GitHub issue with:
   - Your OS and version
   - LLVM version (`llvm-config --version`)
   - Error messages
   - Steps to reproduce

---

**Ready to compile?** Install LLVM and let's build the second-fastest language! 🚀
