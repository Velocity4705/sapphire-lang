#pragma once

#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <functional>
#include <type_traits>

namespace sapphire {
namespace stdlib {
namespace Error {

/**
 * StackFrame - Represents a single frame in a call stack
 */
struct StackFrame {
    std::string function_name;
    std::string file_name;
    size_t line_number;
    size_t column_number;
    std::string source_line;
    
    StackFrame(const std::string& func, const std::string& file, 
               size_t line, size_t col = 0, const std::string& source = "")
        : function_name(func), file_name(file), line_number(line), 
          column_number(col), source_line(source) {}
    
    std::string to_string() const;
};

/**
 * StackTrace - Represents a complete call stack
 */
class StackTrace {
private:
    std::vector<StackFrame> frames;
    
public:
    StackTrace() = default;
    explicit StackTrace(const std::vector<StackFrame>& frames);
    
    // Frame management
    void add_frame(const StackFrame& frame);
    void add_frame(const std::string& function, const std::string& file, size_t line);
    const std::vector<StackFrame>& get_frames() const { return frames; }
    
    // Output
    std::string to_string() const;
    void print() const;
    
    // Utilities
    size_t depth() const { return frames.size(); }
    bool empty() const { return frames.empty(); }
    void clear() { frames.clear(); }
};

/**
 * SapphireError - Base exception class with enhanced diagnostics
 */
class SapphireError : public std::exception {
protected:
    std::string message;
    std::string error_code;
    StackTrace stack_trace;
    std::vector<std::string> suggestions;
    
public:
    SapphireError(const std::string& msg, const std::string& code = "");
    SapphireError(const std::string& msg, const StackTrace& trace, const std::string& code = "");
    
    // Exception interface
    const char* what() const noexcept override;
    
    // Enhanced diagnostics
    const std::string& get_message() const { return message; }
    const std::string& get_error_code() const { return error_code; }
    const StackTrace& get_stack_trace() const { return stack_trace; }
    const std::vector<std::string>& get_suggestions() const { return suggestions; }
    
    // Suggestion management
    void add_suggestion(const std::string& suggestion);
    void set_suggestions(const std::vector<std::string>& suggestions);
    
    // Formatted output
    std::string format_error() const;
    void print_error() const;
};

/**
 * Specific error types
 */
class RuntimeError : public SapphireError {
public:
    RuntimeError(const std::string& msg) : SapphireError(msg, "RUNTIME_ERROR") {}
    RuntimeError(const std::string& msg, const StackTrace& trace) 
        : SapphireError(msg, trace, "RUNTIME_ERROR") {}
};

class TypeError : public SapphireError {
public:
    TypeError(const std::string& msg) : SapphireError(msg, "TYPE_ERROR") {}
    TypeError(const std::string& msg, const StackTrace& trace) 
        : SapphireError(msg, trace, "TYPE_ERROR") {}
};

class ValueError : public SapphireError {
public:
    ValueError(const std::string& msg) : SapphireError(msg, "VALUE_ERROR") {}
    ValueError(const std::string& msg, const StackTrace& trace) 
        : SapphireError(msg, trace, "VALUE_ERROR") {}
};

class IOError : public SapphireError {
public:
    IOError(const std::string& msg) : SapphireError(msg, "IO_ERROR") {}
    IOError(const std::string& msg, const StackTrace& trace) 
        : SapphireError(msg, trace, "IO_ERROR") {}
};

/**
 * ErrorHandler - Global error handling and reporting
 */
class ErrorHandler {
private:
    static std::function<void(const SapphireError&)> error_callback;
    static bool show_stack_traces;
    static bool show_suggestions;
    
public:
    // Configuration
    static void set_error_callback(std::function<void(const SapphireError&)> callback);
    static void enable_stack_traces(bool enable = true);
    static void enable_suggestions(bool enable = true);
    
    // Error handling
    static void handle_error(const SapphireError& error);
    static void report_error(const std::string& message, const std::string& file = "", size_t line = 0);
    
    // Stack trace capture
    static StackTrace capture_stack_trace();
    static void push_frame(const std::string& function, const std::string& file, size_t line);
    static void pop_frame();
};

/**
 * Result<T> - Type-safe error handling without exceptions
 */
template<typename T>
class Result {
private:
    bool is_ok_;
    T value_;
    std::string error_message_;
    
    // Private constructor for error results
    Result(bool ok, const T& val, const std::string& err) 
        : is_ok_(ok), value_(val), error_message_(err) {}
    
public:
    // Constructors
    Result(const T& value) : is_ok_(true), value_(value) {}
    Result(T&& value) : is_ok_(true), value_(std::move(value)) {}
    
    // Static factory for error results
    static Result<T> error(const std::string& message) {
        return Result<T>(false, T{}, message);
    }
    
    // Status checking
    bool is_ok() const { return is_ok_; }
    bool is_error() const { return !is_ok_; }
    
    // Value access
    const T& value() const {
        if (!is_ok_) {
            throw RuntimeError("Attempted to access value of error result: " + error_message_);
        }
        return value_;
    }
    
    T& value() {
        if (!is_ok_) {
            throw RuntimeError("Attempted to access value of error result: " + error_message_);
        }
        return value_;
    }
    
    const T& value_or(const T& default_value) const {
        return is_ok_ ? value_ : default_value;
    }
    
    // Error access
    const std::string& get_error() const {
        if (is_ok_) {
            throw RuntimeError("Attempted to access error of ok result");
        }
        return error_message_;
    }
    
    // Unwrap (throws on error)
    T unwrap() const {
        if (!is_ok_) {
            throw RuntimeError("Result unwrap failed: " + error_message_);
        }
        return value_;
    }
    
    // Map operations
    template<typename U>
    Result<U> map(std::function<U(const T&)> func) const {
        if (is_ok_) {
            return Result<U>(func(value_));
        } else {
            return Result<U>::error(error_message_);
        }
    }
    
    Result<T> map_error(std::function<std::string(const std::string&)> func) const {
        if (is_ok_) {
            return *this;
        } else {
            return Result<T>::error(func(error_message_));
        }
    }
};

// Helper macros for error handling
#define SAPPHIRE_TRY(expr) \
    do { \
        auto result = (expr); \
        if (result.is_error()) { \
            return Result<decltype(result.value())>(result.error()); \
        } \
    } while(0)

#define SAPPHIRE_THROW(error_type, message) \
    throw error_type(message, ErrorHandler::capture_stack_trace())

// Factory functions
std::unique_ptr<StackTrace> create_stack_trace();
std::unique_ptr<SapphireError> create_error(const std::string& message, const std::string& code = "");

} // namespace Error
} // namespace stdlib
} // namespace sapphire

// C API
extern "C" {
    // Error handling
    void sapphire_error_set_callback(void (*callback)(const char* message));
    void sapphire_error_enable_stack_traces(bool enable);
    void sapphire_error_report(const char* message, const char* file, size_t line);
    
    // Stack trace
    void sapphire_stack_push_frame(const char* function, const char* file, size_t line);
    void sapphire_stack_pop_frame();
    const char* sapphire_stack_get_trace();
}