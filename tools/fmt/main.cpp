#include "../../src/formatter/formatter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

void print_help() {
    std::cout << "sapphire fmt - Code Formatter\n\n";
    std::cout << "USAGE:\n";
    std::cout << "    sapphire fmt [OPTIONS] <FILE>\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "    --check        Check if file is formatted (don't modify)\n";
    std::cout << "    --indent <n>   Set indent size (default: 4)\n";
    std::cout << "    --help         Show this help\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }
    
    bool check_only = false;
    int indent_size = 4;
    std::string filename;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        } else if (arg == "--check") {
            check_only = true;
        } else if (arg == "--indent") {
            if (i + 1 < argc) {
                indent_size = std::stoi(argv[++i]);
            }
        } else {
            filename = arg;
        }
    }
    
    if (filename.empty()) {
        std::cerr << "Error: No file specified\n";
        return 1;
    }
    
    if (!fs::exists(filename)) {
        std::cerr << "Error: File not found: " << filename << "\n";
        return 1;
    }
    
    sapphire::FormatOptions options;
    options.indent_size = indent_size;
    
    sapphire::Formatter formatter(options);
    
    if (check_only) {
        // Read original file
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string original = buffer.str();
        
        // Format it
        std::string formatted = formatter.format(original);
        
        if (original == formatted) {
            std::cout << "✓ " << filename << " is formatted correctly\n";
            return 0;
        } else {
            std::cout << "✗ " << filename << " needs formatting\n";
            return 1;
        }
    } else {
        // Format in place
        if (formatter.format_file_inplace(filename)) {
            std::cout << "✓ Formatted " << filename << "\n";
            return 0;
        } else {
            std::cerr << "Error: Failed to format " << filename << "\n";
            return 1;
        }
    }
}
