#ifndef SAPPHIRE_EXCEPTION_H
#define SAPPHIRE_EXCEPTION_H

#include <exception>
#include <string>
#include <vector>

namespace sapphire {

// Stack frame information for better error reporting
struct StackFrame {
    std::string function_name;
    std::string file_name;
    int line_number;
    int column_number;
    
    StackFrame(const std::string& func, const std::string& file, int line, int col = 0)
        : function_name(func), file_name(file), line_number(line), column_number(col) {}
        
    std::string toString() const {
        return "  at " + function_name + " (" + file_name + ":" + std::to_string(line_number) + 
               (column_number > 0 ? ":" + std::to_string(column_number) : "") + ")";
    }
};

// Base exception class for all Sapphire exceptions
class SapphireException : public std::exception {
protected:
    std::string message;
    std::string type_name;
    std::vector<StackFrame> stack_trace;
    int line_number;
    int column_number;
    std::string file_name;
    std::string suggestion;
    
public:
    SapphireException(const std::string& msg, const std::string& type)
        : message(msg), type_name(type), line_number(-1), column_number(-1) {}
    
    SapphireException(const std::string& msg, const std::string& type, 
                     const std::string& file, int line, int col = -1)
        : message(msg), type_name(type), line_number(line), column_number(col), file_name(file) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    std::string getTypeName() const { return type_name; }
    std::string getMessage() const { return message; }
    std::string getFileName() const { return file_name; }
    int getLineNumber() const { return line_number; }
    int getColumnNumber() const { return column_number; }
    std::string getSuggestion() const { return suggestion; }
    
    void setLocation(const std::string& file, int line, int col = -1) {
        file_name = file;
        line_number = line;
        column_number = col;
    }
    
    void setSuggestion(const std::string& hint) {
        suggestion = hint;
    }
    
    void addStackFrame(const StackFrame& frame) {
        stack_trace.push_back(frame);
    }
    
    std::string getFullErrorMessage() const {
        std::string full_msg = type_name + ": " + message;
        
        if (!file_name.empty() && line_number > 0) {
            full_msg += "\n  at " + file_name + ":" + std::to_string(line_number);
            if (column_number > 0) {
                full_msg += ":" + std::to_string(column_number);
            }
        }
        
        if (!stack_trace.empty()) {
            full_msg += "\n\nStack trace:";
            for (const auto& frame : stack_trace) {
                full_msg += "\n" + frame.toString();
            }
        }
        
        if (!suggestion.empty()) {
            full_msg += "\n\nSuggestion: " + suggestion;
        }
        
        return full_msg;
    }
    
    const std::vector<StackFrame>& getStackTrace() const { return stack_trace; }
};

// Built-in exception types

class RuntimeError : public SapphireException {
public:
    explicit RuntimeError(const std::string& msg) 
        : SapphireException(msg, "RuntimeError") {}
        
    RuntimeError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "RuntimeError", file, line, col) {}
};

class DivisionByZeroError : public SapphireException {
public:
    explicit DivisionByZeroError(const std::string& msg) 
        : SapphireException(msg, "DivisionByZeroError") {}
        
    DivisionByZeroError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "DivisionByZeroError", file, line, col) {}
};

class FileNotFoundError : public SapphireException {
public:
    explicit FileNotFoundError(const std::string& msg) 
        : SapphireException(msg, "FileNotFoundError") {}
        
    FileNotFoundError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "FileNotFoundError", file, line, col) {}
};

class IndexError : public SapphireException {
public:
    explicit IndexError(const std::string& msg) 
        : SapphireException(msg, "IndexError") {}
        
    IndexError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "IndexError", file, line, col) {}
};

class TypeError : public SapphireException {
public:
    explicit TypeError(const std::string& msg) 
        : SapphireException(msg, "TypeError") {}
        
    TypeError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "TypeError", file, line, col) {}
};

class ValueError : public SapphireException {
public:
    explicit ValueError(const std::string& msg) 
        : SapphireException(msg, "ValueError") {}
        
    ValueError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "ValueError", file, line, col) {}
};

class PermissionError : public SapphireException {
public:
    explicit PermissionError(const std::string& msg)
        : SapphireException(msg, "PermissionError") {}
        
    PermissionError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "PermissionError", file, line, col) {}
};

// JSON-specific errors
class JSONParseError : public SapphireException {
public:
    explicit JSONParseError(const std::string& msg)
        : SapphireException(msg, "JSONParseError") {}
        
    JSONParseError(const std::string& msg, const std::string& file, int line, int col = -1)
        : SapphireException(msg, "JSONParseError", file, line, col) {}
};

} // namespace sapphire

#endif // SAPPHIRE_EXCEPTION_H
