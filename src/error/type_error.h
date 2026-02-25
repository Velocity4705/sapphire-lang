#ifndef SAPPHIRE_TYPE_ERROR_H
#define SAPPHIRE_TYPE_ERROR_H

#include "../types/type.h"
#include <string>
#include <vector>
#include <memory>

namespace sapphire {

// Error severity
enum class ErrorSeverity {
    ERROR,
    WARNING,
    NOTE
};

// Source location
struct SourceLocation {
    int line;
    int column;
    std::string filename;
    
    SourceLocation(int l = 0, int c = 0, const std::string& f = "")
        : line(l), column(c), filename(f) {}
    
    std::string toString() const {
        if (filename.empty()) {
            return "line " + std::to_string(line) + ", column " + std::to_string(column);
        }
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// Type error with rich information
class TypeErrorInfo {
private:
    ErrorSeverity severity;
    std::string message;
    SourceLocation location;
    std::vector<std::string> notes;
    std::vector<std::string> suggestions;
    std::shared_ptr<Type> expected_type;
    std::shared_ptr<Type> actual_type;
    
public:
    TypeErrorInfo(ErrorSeverity sev, const std::string& msg, const SourceLocation& loc)
        : severity(sev), message(msg), location(loc) {}
    
    void addNote(const std::string& note) {
        notes.push_back(note);
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
    
    void setExpectedType(std::shared_ptr<Type> type) {
        expected_type = type;
    }
    
    void setActualType(std::shared_ptr<Type> type) {
        actual_type = type;
    }
    
    std::string format() const {
        std::string result;
        
        // Severity and location
        switch (severity) {
            case ErrorSeverity::ERROR:
                result += "error: ";
                break;
            case ErrorSeverity::WARNING:
                result += "warning: ";
                break;
            case ErrorSeverity::NOTE:
                result += "note: ";
                break;
        }
        
        result += message + "\n";
        result += "  --> " + location.toString() + "\n";
        
        // Type mismatch details
        if (expected_type && actual_type) {
            result += "\n";
            result += "  expected: " + expected_type->toString() + "\n";
            result += "  found:    " + actual_type->toString() + "\n";
        }
        
        // Notes
        for (const auto& note : notes) {
            result += "\n";
            result += "note: " + note + "\n";
        }
        
        // Suggestions
        if (!suggestions.empty()) {
            result += "\n";
            result += "help: ";
            for (size_t i = 0; i < suggestions.size(); i++) {
                if (i > 0) result += "      ";
                result += suggestions[i] + "\n";
            }
        }
        
        return result;
    }
    
    ErrorSeverity getSeverity() const { return severity; }
    std::string getMessage() const { return message; }
    SourceLocation getLocation() const { return location; }
};

// Error reporter
class ErrorReporter {
private:
    std::vector<TypeErrorInfo> errors;
    bool has_errors = false;
    
public:
    void reportError(const TypeErrorInfo& error) {
        errors.push_back(error);
        if (error.getSeverity() == ErrorSeverity::ERROR) {
            has_errors = true;
        }
    }
    
    void reportTypeMismatch(const SourceLocation& loc,
                           std::shared_ptr<Type> expected,
                           std::shared_ptr<Type> actual,
                           const std::string& context = "") {
        TypeErrorInfo error(ErrorSeverity::ERROR, 
                           "type mismatch" + (context.empty() ? "" : " in " + context),
                           loc);
        error.setExpectedType(expected);
        error.setActualType(actual);
        
        // Add helpful suggestions
        if (expected->isFloat() && actual->isInt()) {
            error.addSuggestion("convert to float: " + actual->toString() + " as float");
        } else if (expected->isString() && (actual->isInt() || actual->isFloat())) {
            error.addSuggestion("convert to string: str(" + actual->toString() + ")");
        }
        
        reportError(error);
    }
    
    void reportUndefinedVariable(const SourceLocation& loc, const std::string& name) {
        TypeErrorInfo error(ErrorSeverity::ERROR,
                           "undefined variable '" + name + "'",
                           loc);
        error.addSuggestion("check the variable name for typos");
        error.addSuggestion("make sure the variable is defined before use");
        reportError(error);
    }
    
    void reportArityMismatch(const SourceLocation& loc,
                            size_t expected, size_t actual) {
        TypeErrorInfo error(ErrorSeverity::ERROR,
                           "function argument count mismatch",
                           loc);
        error.addNote("expected " + std::to_string(expected) + " arguments");
        error.addNote("found " + std::to_string(actual) + " arguments");
        reportError(error);
    }
    
    void reportConstraintViolation(const SourceLocation& loc,
                                   std::shared_ptr<Type> type,
                                   const std::string& constraint) {
        TypeErrorInfo error(ErrorSeverity::ERROR,
                           "type constraint violation",
                           loc);
        error.addNote("type " + type->toString() + " does not satisfy constraint " + constraint);
        reportError(error);
    }
    
    bool hasErrors() const {
        return has_errors;
    }
    
    size_t errorCount() const {
        size_t count = 0;
        for (const auto& e : errors) {
            if (e.getSeverity() == ErrorSeverity::ERROR) {
                count++;
            }
        }
        return count;
    }
    
    void printErrors() const {
        for (const auto& error : errors) {
            std::cerr << error.format() << std::endl;
        }
    }
    
    void clear() {
        errors.clear();
        has_errors = false;
    }
    
    const std::vector<TypeErrorInfo>& getErrors() const {
        return errors;
    }
};

} // namespace sapphire

#endif // SAPPHIRE_TYPE_ERROR_H
