#ifndef SAPPHIRE_EXCEPTION_H
#define SAPPHIRE_EXCEPTION_H

#include <exception>
#include <string>

namespace sapphire {

// Base exception class for all Sapphire exceptions
class SapphireException : public std::exception {
protected:
    std::string message;
    std::string type_name;
    
public:
    SapphireException(const std::string& msg, const std::string& type)
        : message(msg), type_name(type) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    std::string getTypeName() const { return type_name; }
    std::string getMessage() const { return message; }
};

// Built-in exception types

class RuntimeError : public SapphireException {
public:
    explicit RuntimeError(const std::string& msg) 
        : SapphireException(msg, "RuntimeError") {}
};

class DivisionByZeroError : public SapphireException {
public:
    explicit DivisionByZeroError(const std::string& msg) 
        : SapphireException(msg, "DivisionByZeroError") {}
};

class FileNotFoundError : public SapphireException {
public:
    explicit FileNotFoundError(const std::string& msg) 
        : SapphireException(msg, "FileNotFoundError") {}
};

class IndexError : public SapphireException {
public:
    explicit IndexError(const std::string& msg) 
        : SapphireException(msg, "IndexError") {}
};

class TypeError : public SapphireException {
public:
    explicit TypeError(const std::string& msg) 
        : SapphireException(msg, "TypeError") {}
};

class ValueError : public SapphireException {
public:
    explicit ValueError(const std::string& msg) 
        : SapphireException(msg, "ValueError") {}
};

} // namespace sapphire

#endif // SAPPHIRE_EXCEPTION_H
