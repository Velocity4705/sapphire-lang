#include "formatter.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace sapphire {

Formatter::Formatter(const FormatOptions& options) 
    : options_(options), current_indent_(0) {}

void Formatter::add_indent() {
    for (int i = 0; i < current_indent_ * options_.indent_size; i++) {
        result_ += ' ';
    }
}

void Formatter::add_line(const std::string& line) {
    if (!line.empty()) {
        add_indent();
        result_ += line;
    }
    result_ += '\n';
}

void Formatter::increase_indent() {
    current_indent_++;
}

void Formatter::decrease_indent() {
    if (current_indent_ > 0) {
        current_indent_--;
    }
}

std::string Formatter::format(const std::string& code) {
    result_.clear();
    current_indent_ = 0;
    
    std::istringstream stream(code);
    std::string line;
    bool in_string = false;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) {
            // Empty line
            result_ += '\n';
            continue;
        }
        
        line = line.substr(start);
        
        // Handle comments
        if (line[0] == '#') {
            add_line(line);
            continue;
        }
        
        // Handle closing braces
        if (line[0] == '}') {
            decrease_indent();
        }
        
        // Format the line
        std::string formatted;
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            
            // Handle strings
            if (c == '"' && (i == 0 || line[i-1] != '\\')) {
                in_string = !in_string;
                formatted += c;
                continue;
            }
            
            if (in_string) {
                formatted += c;
                continue;
            }
            
            // Add spaces around operators
            if (options_.space_around_operators) {
                if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' ||
                    c == '<' || c == '>' || c == '!') {
                    if (!formatted.empty() && formatted.back() != ' ') {
                        formatted += ' ';
                    }
                    formatted += c;
                    if (i + 1 < line.length() && line[i+1] != ' ' && line[i+1] != '=') {
                        formatted += ' ';
                    }
                    continue;
                }
            }
            
            // Add space after comma
            if (options_.space_after_comma && c == ',') {
                formatted += c;
                if (i + 1 < line.length() && line[i+1] != ' ') {
                    formatted += ' ';
                }
                continue;
            }
            
            // Handle braces
            if (c == '{') {
                if (options_.newline_before_brace) {
                    add_line(formatted);
                    formatted.clear();
                    add_line("{");
                } else {
                    if (!formatted.empty() && formatted.back() != ' ') {
                        formatted += ' ';
                    }
                    formatted += c;
                    add_line(formatted);
                    formatted.clear();
                }
                increase_indent();
                continue;
            }
            
            formatted += c;
        }
        
        if (!formatted.empty()) {
            add_line(formatted);
        }
    }
    
    return result_;
}

std::string Formatter::format_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return format(buffer.str());
}

bool Formatter::format_file_inplace(const std::string& filename) {
    std::string formatted = format_file(filename);
    if (formatted.empty()) {
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << formatted;
    return true;
}

} // namespace sapphire
