#include "error.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace sapphire {
namespace stdlib {
namespace Error {

//=============================================================================
// StackFrame Implementation
//=============================================================================

std::string StackFrame::to_string() const {
    std::stringstream ss;
    ss << "  at " << function_name;
    
    if (!file_name.empty()) {
        ss << " (" << file_name;
        if (line_number > 0) {
            ss << ":" << line_number;
            if (column_number > 0) {
                ss << ":" << column_number;
            }
        }
        ss << ")";
    }
    
    if (!source_line.empty()) {
        ss << "\n    " << source_line;
        if (column_number > 0) {
            ss << "\n    " << std::string(column_number - 1, ' ') << "^";
        }
    }
    
    return ss.str();
}

//=============================================================================
// StackTrace Implementation
//=============================================================================

StackTrace::StackTrace(const std::vector<StackFrame>& frames) : frames(frames) {}

void StackTrace::add_frame(const StackFrame& frame) {
    frames.push_back(frame);
}

void StackTrace::add_frame(const std::string& function, const std::string& file, size_t line) {
    frames.emplace_back(function, file, line);
}

std::string StackTrace::to_string() const {
    if (frames.empty()) {
        return "  (no stack trace available)";
    }
    
    std::stringstream ss;
    ss << "Stack trace:\n";
    
    for (const auto& frame : frames) {
        ss << frame.to_string() << "\n";
    }
    
    return ss.str();
}

void StackTrace::print() const {
    std::cerr << to_string() << std::endl;
}

//=============================================================================
// SapphireError Implementation
//=============================================================================

SapphireError::SapphireError(const std::string& msg, const std::string& code) 
    : message(msg), error_code(code) {}

SapphireError::SapphireError(const std::string& msg, const StackTrace& trace, const std::string& code) 
    : message(msg), error_code(code), stack_trace(trace) {}

const char* SapphireError::what() const noexcept {
    return message.c_str();
}

void SapphireError::add_suggestion(const std::string& suggestion) {
    suggestions.push_back(suggestion);
}

void SapphireError::set_suggestions(const std::vector<std::string>& suggestions) {
    this->suggestions = suggestions;
}

std::string SapphireError::format_error() const {
    std::stringstream ss;
    
    // Error header
    ss << "Error";
    if (!error_code.empty()) {
        ss << " [" << error_code << "]";
    }
    ss << ": " << message << "\n";
    
    // Stack trace
    if (!stack_trace.empty()) {
        ss << "\n" << stack_trace.to_string();
    }
    
    // Suggestions
    if (!suggestions.empty()) {
        ss << "\nSuggestions:\n";
        for (size_t i = 0; i < suggestions.size(); ++i) {
            ss << "  " << (i + 1) << ". " << suggestions[i] << "\n";
        }
    }
    
    return ss.str();
}

void SapphireError::print_error() const {
    std::cerr << format_error() << std::endl;
}

//=============================================================================
// ErrorHandler Implementation
//=============================================================================

std::function<void(const SapphireError&)> ErrorHandler::error_callback = nullptr;
bool ErrorHandler::show_stack_traces = true;
bool ErrorHandler::show_suggestions = true;

void ErrorHandler::set_error_callback(std::function<void(const SapphireError&)> callback) {
    error_callback = callback;
}

void ErrorHandler::enable_stack_traces(bool enable) {
    show_stack_traces = enable;
}

void ErrorHandler::enable_suggestions(bool enable) {
    show_suggestions = enable;
}

void ErrorHandler::handle_error(const SapphireError& error) {
    if (error_callback) {
        error_callback(error);
    } else {
        // Default error handling
        std::cerr << "\n" << std::string(60, '=') << "\n";
        std::cerr << "SAPPHIRE ERROR\n";
        std::cerr << std::string(60, '=') << "\n";
        
        std::cerr << "Message: " << error.get_message() << "\n";
        
        if (!error.get_error_code().empty()) {
            std::cerr << "Code: " << error.get_error_code() << "\n";
        }
        
        if (show_stack_traces && !error.get_stack_trace().empty()) {
            std::cerr << "\n" << error.get_stack_trace().to_string();
        }
        
        if (show_suggestions && !error.get_suggestions().empty()) {
            std::cerr << "\nSuggestions:\n";
            for (size_t i = 0; i < error.get_suggestions().size(); ++i) {
                std::cerr << "  " << (i + 1) << ". " << error.get_suggestions()[i] << "\n";
            }
        }
        
        std::cerr << std::string(60, '=') << "\n\n";
    }
}

void ErrorHandler::report_error(const std::string& message, const std::string& file, size_t line) {
    StackTrace trace;
    if (!file.empty()) {
        trace.add_frame("(unknown)", file, line);
    }
    
    SapphireError error(message, trace);
    handle_error(error);
}

// Simple stack trace implementation (would be enhanced with platform-specific code)
static thread_local std::vector<StackFrame> current_stack;

StackTrace ErrorHandler::capture_stack_trace() {
    return StackTrace(current_stack);
}

void ErrorHandler::push_frame(const std::string& function, const std::string& file, size_t line) {
    current_stack.emplace_back(function, file, line);
}

void ErrorHandler::pop_frame() {
    if (!current_stack.empty()) {
        current_stack.pop_back();
    }
}

//=============================================================================
// Factory Functions
//=============================================================================

std::unique_ptr<StackTrace> create_stack_trace() {
    return std::make_unique<StackTrace>();
}

std::unique_ptr<SapphireError> create_error(const std::string& message, const std::string& code) {
    return std::make_unique<SapphireError>(message, code);
}

} // namespace Error
} // namespace stdlib
} // namespace sapphire

//=============================================================================
// C API Implementation
//=============================================================================

using namespace sapphire::stdlib::Error;

extern "C" {
    static thread_local char g_error_string_buffer[8192];
    static void (*g_error_callback)(const char*) = nullptr;
    
    void sapphire_error_set_callback(void (*callback)(const char* message)) {
        g_error_callback = callback;
        
        if (callback) {
            ErrorHandler::set_error_callback([](const SapphireError& error) {
                g_error_callback(error.format_error().c_str());
            });
        } else {
            ErrorHandler::set_error_callback(nullptr);
        }
    }
    
    void sapphire_error_enable_stack_traces(bool enable) {
        ErrorHandler::enable_stack_traces(enable);
    }
    
    void sapphire_error_report(const char* message, const char* file, size_t line) {
        if (!message) return;
        
        std::string file_str = file ? file : "";
        ErrorHandler::report_error(message, file_str, line);
    }
    
    void sapphire_stack_push_frame(const char* function, const char* file, size_t line) {
        if (!function) return;
        
        std::string func_str = function;
        std::string file_str = file ? file : "";
        ErrorHandler::push_frame(func_str, file_str, line);
    }
    
    void sapphire_stack_pop_frame() {
        ErrorHandler::pop_frame();
    }
    
    const char* sapphire_stack_get_trace() {
        StackTrace trace = ErrorHandler::capture_stack_trace();
        std::string trace_str = trace.to_string();
        
        strncpy(g_error_string_buffer, trace_str.c_str(), sizeof(g_error_string_buffer) - 1);
        g_error_string_buffer[sizeof(g_error_string_buffer) - 1] = '\0';
        
        return g_error_string_buffer;
    }
}