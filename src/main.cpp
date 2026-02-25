#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "semantic/type_checker.h"
#include "semantic/type_inference.h"
#include "codegen/llvm_codegen.h"
#include "codegen/llvm_codegen.h"

void printUsage(const char* program) {
    (void)program;
    std::cout << "Sapphire Programming Language 💎\n";
    std::cout << "Combining the best of Python, C++, Rust, Go, and more\n\n";
    std::cout << "Usage:\n";
    std::cout << "  sapp <file.spp>              # Run a program (interpreter)\n";
    std::cout << "  sapp compile <file.spp>      # Compile and show LLVM IR\n";
    std::cout << "  sapp --version               # Show version\n";
    std::cout << "  sapp --help                  # Show this help\n";
    std::cout << "\nNote: 'sapphire' is an alias for 'sapp'\n";
    std::cout << "\nMilestone 3 - Phase 2: Expression code generation enabled!\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string arg = argv[1];
    
    if (arg == "--version") {
        std::cout << "Sapphire v0.3.0-alpha (Milestone 3 - Phase 2: Code Generation)\n";
        return 0;
    }
    
    if (arg == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    // Check for compile command
    bool compile_mode = false;
    std::string filename;
    
    if (arg == "compile") {
        if (argc < 3) {
            std::cerr << "Error: compile command requires a filename\n";
            std::cerr << "Usage: sapp compile <file.spp>\n";
            return 1;
        }
        compile_mode = true;
        filename = argv[2];
    } else {
        filename = argv[1];
    }
    
    // Read source file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    try {
        // Lexer - tokenize source
        sapphire::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        // Parser - build AST
        sapphire::Parser parser(tokens);
        auto statements = parser.parse();
        
        if (compile_mode) {
            // Code generation mode
            std::cout << "=== Compiling " << filename << " ===\n\n";
            
            sapphire::LLVMCodeGen codegen(filename);
            codegen.generate(statements);
            
            std::cout << "\n=== Generated LLVM IR ===\n\n";
            codegen.printIR();
            
            std::cout << "\n=== Compilation successful! ===\n";
        } else {
            // Interpreter mode
            // Type checker (optional for now)
            bool enable_type_checking = false;
            if (enable_type_checking) {
                sapphire::TypeChecker type_checker;
                type_checker.check(statements);
            }
            
            // Interpreter - execute
            sapphire::Interpreter interpreter;
            interpreter.interpret(statements);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
