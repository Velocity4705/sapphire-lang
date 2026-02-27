# Error Handling Design Document

## Architecture Overview

The error handling system will be implemented using C++ exceptions internally, with a Sapphire-level exception type system that maps to the internal implementation.

## Component Design

### 1. Exception Type System

#### Exception Base Class
```cpp
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
```

#### Built-in Exception Types
```cpp
class RuntimeError : public SapphireException {
public:
    RuntimeError(const std::string& msg) 
        : SapphireException(msg, "RuntimeError") {}
};

class DivisionByZeroError : public SapphireException {
public:
    DivisionByZeroError(const std::string& msg) 
        : SapphireException(msg, "DivisionByZeroError") {}
};

class FileNotFoundError : public SapphireException {
public:
    FileNotFoundError(const std::string& msg) 
        : SapphireException(msg, "FileNotFoundError") {}
};

class IndexError : public SapphireException {
public:
    IndexError(const std::string& msg) 
        : SapphireException(msg, "IndexError") {}
};

class TypeError : public SapphireException {
public:
    TypeError(const std::string& msg) 
        : SapphireException(msg, "TypeError") {}
};

class ValueError : public SapphireException {
public:
    ValueError(const std::string& msg) 
        : SapphireException(msg, "ValueError") {}
};
```

### 2. AST Nodes

#### TryStmt
```cpp
class CatchClause {
public:
    std::string exception_type;  // Empty string means catch all
    std::string variable_name;   // Variable to bind exception to
    std::vector<std::unique_ptr<Stmt>> body;
    
    CatchClause(const std::string& type, const std::string& var,
                std::vector<std::unique_ptr<Stmt>> b)
        : exception_type(type), variable_name(var), body(std::move(b)) {}
};

class TryStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> try_body;
    std::vector<CatchClause> catch_clauses;
    std::vector<std::unique_ptr<Stmt>> finally_body;
    
    TryStmt(std::vector<std::unique_ptr<Stmt>> try_b,
            std::vector<CatchClause> catches,
            std::vector<std::unique_ptr<Stmt>> finally_b)
        : try_body(std::move(try_b)),
          catch_clauses(std::move(catches)),
          finally_body(std::move(finally_b)) {}
    
    void accept(StmtVisitor& visitor) override {
        visitor.visitTryStmt(*this);
    }
};
```

#### ThrowStmt
```cpp
class ThrowStmt : public Stmt {
public:
    std::string exception_type;
    std::unique_ptr<Expr> message;
    
    ThrowStmt(const std::string& type, std::unique_ptr<Expr> msg)
        : exception_type(type), message(std::move(msg)) {}
    
    void accept(StmtVisitor& visitor) override {
        visitor.visitThrowStmt(*this);
    }
};
```

### 3. Parser Implementation

#### Parsing Try-Catch-Finally
```cpp
std::unique_ptr<Stmt> Parser::parseTryStmt() {
    consume(TokenType::TRY, "Expected 'try'");
    consume(TokenType::COLON, "Expected ':' after 'try'");
    consume(TokenType::NEWLINE, "Expected newline after ':'");
    consume(TokenType::INDENT, "Expected indentation");
    
    // Parse try body
    std::vector<std::unique_ptr<Stmt>> try_body;
    while (!check(TokenType::DEDENT)) {
        try_body.push_back(parseStatement());
    }
    consume(TokenType::DEDENT, "Expected dedent");
    
    // Parse catch clauses
    std::vector<CatchClause> catch_clauses;
    while (match(TokenType::CATCH)) {
        std::string exception_type;
        std::string variable_name;
        
        // Check for exception type
        if (check(TokenType::IDENTIFIER) && peek(1).type == TokenType::AS) {
            exception_type = advance().lexeme;
            consume(TokenType::AS, "Expected 'as'");
            variable_name = consume(TokenType::IDENTIFIER, "Expected variable name").lexeme;
        } else if (check(TokenType::IDENTIFIER)) {
            variable_name = advance().lexeme;
        }
        
        consume(TokenType::COLON, "Expected ':' after catch");
        consume(TokenType::NEWLINE, "Expected newline");
        consume(TokenType::INDENT, "Expected indentation");
        
        std::vector<std::unique_ptr<Stmt>> catch_body;
        while (!check(TokenType::DEDENT)) {
            catch_body.push_back(parseStatement());
        }
        consume(TokenType::DEDENT, "Expected dedent");
        
        catch_clauses.emplace_back(exception_type, variable_name, std::move(catch_body));
    }
    
    // Parse finally clause
    std::vector<std::unique_ptr<Stmt>> finally_body;
    if (match(TokenType::FINALLY)) {
        consume(TokenType::COLON, "Expected ':' after 'finally'");
        consume(TokenType::NEWLINE, "Expected newline");
        consume(TokenType::INDENT, "Expected indentation");
        
        while (!check(TokenType::DEDENT)) {
            finally_body.push_back(parseStatement());
        }
        consume(TokenType::DEDENT, "Expected dedent");
    }
    
    return std::make_unique<TryStmt>(std::move(try_body),
                                     std::move(catch_clauses),
                                     std::move(finally_body));
}
```

#### Parsing Throw Statement
```cpp
std::unique_ptr<Stmt> Parser::parseThrowStmt() {
    consume(TokenType::THROW, "Expected 'throw'");
    
    std::string exception_type = "RuntimeError";  // Default
    std::unique_ptr<Expr> message;
    
    if (check(TokenType::IDENTIFIER) && peek(1).type == TokenType::LEFT_PAREN) {
        exception_type = advance().lexeme;
        consume(TokenType::LEFT_PAREN, "Expected '('");
        message = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
    } else {
        message = parseExpression();
    }
    
    return std::make_unique<ThrowStmt>(exception_type, std::move(message));
}
```

### 4. Interpreter Implementation

#### Executing Try-Catch-Finally
```cpp
void Interpreter::visitTryStmt(TryStmt& stmt) {
    bool exception_caught = false;
    SapphireException* caught_exception = nullptr;
    
    try {
        // Execute try block
        for (auto& s : stmt.try_body) {
            s->accept(*this);
        }
    } catch (const SapphireException& e) {
        // Try to match exception with catch clauses
        for (auto& catch_clause : stmt.catch_clauses) {
            if (catch_clause.exception_type.empty() || 
                catch_clause.exception_type == e.getTypeName()) {
                
                exception_caught = true;
                
                // Bind exception to variable if specified
                if (!catch_clause.variable_name.empty()) {
                    environment->define(catch_clause.variable_name, 
                                      std::string(e.getMessage()));
                }
                
                // Execute catch block
                for (auto& s : catch_clause.body) {
                    s->accept(*this);
                }
                
                break;  // Only execute first matching catch
            }
        }
        
        // If no catch clause matched, save exception to re-throw
        if (!exception_caught) {
            caught_exception = new SapphireException(e);
        }
    }
    
    // Always execute finally block
    if (!stmt.finally_body.empty()) {
        for (auto& s : stmt.finally_body) {
            s->accept(*this);
        }
    }
    
    // Re-throw if exception wasn't caught
    if (caught_exception) {
        auto temp = *caught_exception;
        delete caught_exception;
        throw temp;
    }
}
```

#### Executing Throw Statement
```cpp
void Interpreter::visitThrowStmt(ThrowStmt& stmt) {
    Value message_value = evaluateExpr(*stmt.message);
    std::string message = valueToString(message_value);
    
    if (stmt.exception_type == "RuntimeError") {
        throw RuntimeError(message);
    } else if (stmt.exception_type == "DivisionByZeroError") {
        throw DivisionByZeroError(message);
    } else if (stmt.exception_type == "FileNotFoundError") {
        throw FileNotFoundError(message);
    } else if (stmt.exception_type == "IndexError") {
        throw IndexError(message);
    } else if (stmt.exception_type == "TypeError") {
        throw TypeError(message);
    } else if (stmt.exception_type == "ValueError") {
        throw ValueError(message);
    } else {
        throw RuntimeError(message);  // Default to RuntimeError
    }
}
```

### 5. Integration with Existing Code

#### Update Division Operation
```cpp
if (expr.op == "/") {
    double l = std::holds_alternative<int>(left) ? std::get<int>(left) : std::get<double>(left);
    double r = std::holds_alternative<int>(right) ? std::get<int>(right) : std::get<double>(right);
    
    if (r == 0.0) {
        throw DivisionByZeroError("Division by zero");
    }
    
    last_value = l / r;
}
```

#### Update Array Indexing
```cpp
if (idx < 0 || idx >= static_cast<int>(array->elements.size())) {
    throw IndexError("Array index out of bounds: " + std::to_string(idx));
}
```

#### Update File Operations
```cpp
if (!file.is_open()) {
    throw FileNotFoundError("Failed to open file: " + filename);
}
```

## Lexer Updates

Add new keywords:
```cpp
keywords["try"] = TokenType::TRY;
keywords["catch"] = TokenType::CATCH;
keywords["finally"] = TokenType::FINALLY;
keywords["throw"] = TokenType::THROW;
keywords["as"] = TokenType::AS;
```

## Token Types

Add to token.h:
```cpp
enum class TokenType {
    // ... existing tokens ...
    TRY, CATCH, FINALLY, THROW, AS,
    // ... rest of tokens ...
};
```

## Error Messages

All error messages should be clear and helpful:
- Include context (what operation failed)
- Include relevant values (e.g., index that was out of bounds)
- Suggest possible fixes when appropriate

## Performance Considerations

1. Exception handling should have minimal overhead when no exceptions occur
2. Use C++ exceptions internally for efficient propagation
3. Avoid unnecessary exception object creation

## Testing Strategy

1. Unit tests for each exception type
2. Integration tests for try-catch-finally combinations
3. Nested try-catch tests
4. Exception propagation tests
5. Finally block execution order tests

## Migration Path

Existing code will continue to work:
- Runtime errors will still throw exceptions
- They can now be caught with try-catch
- No breaking changes to existing functionality

## Future Enhancements (v1.1+)

1. Custom exception classes
2. Exception chaining (caused by)
3. Stack traces
4. Result<T, E> type for functional error handling
5. Option<T> type for nullable values
