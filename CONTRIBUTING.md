# Contributing to Sapphire

Thank you for your interest in contributing to Sapphire! This project aims to combine the best features from all major programming languages.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/sapphire-lang.git`
3. Create a branch: `git checkout -b feature/your-feature`
4. Make your changes
5. Run tests: `make test`
6. Commit: `git commit -m "Add feature X"`
7. Push: `git push origin feature/your-feature`
8. Open a Pull Request

## Development Setup

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.20+
- LLVM 14+ (for code generation)
- Python 3.8+ (for build scripts)

### Building
```bash
make build
./sapphire examples/hello.spp
```

## Project Structure

- `src/lexer/` - Tokenization
- `src/parser/` - Syntax analysis and AST
- `src/semantic/` - Type checking and semantic analysis
- `src/codegen/` - LLVM IR generation
- `src/runtime/` - Garbage collector and runtime
- `src/stdlib/` - Standard library
- `examples/` - Example programs
- `tests/` - Test suite
- `docs/` - Documentation

## Coding Standards

- Follow C++20 best practices
- Use meaningful variable names
- Add comments for complex logic
- Write tests for new features
- Keep functions small and focused
- Use RAII for resource management

## Areas for Contribution

### High Priority
- Lexer and parser implementation
- Type inference engine
- Error message formatting
- Standard library functions
- Documentation and examples

### Medium Priority
- Optimization passes
- Garbage collector tuning
- IDE integration (LSP)
- Package manager
- Debugger support

### Future Work
- JIT compiler
- WASM backend
- Mobile platform support
- Formal verification
- Advanced metaprogramming

## Testing

Write tests for all new features:
```cpp
// tests/test_lexer.cpp
TEST(Lexer, TokenizesIntegers) {
    Lexer lexer("42");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[0].lexeme, "42");
}
```

## Documentation

- Update docs/ when adding features
- Add examples to examples/
- Keep README.md current
- Document public APIs

## Questions?

Open an issue or start a discussion on GitHub.

## License

By contributing, you agree that your contributions will be licensed under the project's license.
