#include "parser.h"
#include <iostream>

namespace sapphire {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        try {
            skipNewlines();
            if (!isAtEnd()) {
                statements.push_back(declaration());
            }
        } catch (const ParseError& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return statements;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    throw error(peek(), message);
}

Token Parser::consumeToken(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

void Parser::skipNewlines() {
    while (match({TokenType::NEWLINE})) {}
}

ParseError Parser::error(const Token& token, const std::string& message) {
    std::string error_msg = "Line " + std::to_string(token.line) + ": " + message;
    return ParseError(error_msg);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;
        
        switch (peek().type) {
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

// Statement parsing

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET, TokenType::CONST})) {
        return varDeclaration();
    }
    if (match({TokenType::FN, TokenType::ASYNC})) {
        return functionDeclaration();
    }
    if (match({TokenType::CLASS})) {
        return classDeclaration();
    }
    if (match({TokenType::IMPORT, TokenType::FROM})) {
        return importDeclaration();
    }
    return statement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    bool is_const = previous().type == TokenType::CONST;
    
    Token name = consumeToken(TokenType::IDENTIFIER, "Expected variable name");
    
    std::string type_name;
    if (match({TokenType::COLON})) {
        Token type = consumeToken(TokenType::IDENTIFIER, "Expected type name");
        type_name = type.lexeme;
    }
    
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    
    skipNewlines();
    
    return std::make_unique<VarDeclStmt>(name.lexeme, type_name, 
                                         std::move(initializer), is_const);
}

std::unique_ptr<Stmt> Parser::functionDeclaration() {
    bool is_async = previous().type == TokenType::ASYNC;
    if (is_async) {
        consume(TokenType::FN, "Expected 'fn' after 'async'");
    }
    
    Token name = consumeToken(TokenType::IDENTIFIER, "Expected function name");
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    std::vector<std::pair<std::string, std::string>> parameters;
    if (!check(TokenType::RPAREN)) {
        do {
            Token param_name = consumeToken(TokenType::IDENTIFIER, "Expected parameter name");
            std::string param_type;
            
            if (match({TokenType::COLON})) {
                Token type = consumeToken(TokenType::IDENTIFIER, "Expected parameter type");
                param_type = type.lexeme;
            }
            
            parameters.push_back({param_name.lexeme, param_type});
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    std::string return_type;
    if (match({TokenType::ARROW})) {
        Token type = consumeToken(TokenType::IDENTIFIER, "Expected return type");
        return_type = type.lexeme;
    }
    
    consume(TokenType::COLON, "Expected ':' before function body");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indented block");
    
    auto body = block();
    
    return std::make_unique<FunctionDecl>(name.lexeme, std::move(parameters),
                                          return_type, std::move(body), is_async);
}

std::unique_ptr<Stmt> Parser::classDeclaration() {
    // class Name[:]
    Token name = consumeToken(TokenType::IDENTIFIER, "Expected class name");
    
    std::string superclass_name;
    if (match({TokenType::LPAREN})) {
        Token base = consumeToken(TokenType::IDENTIFIER, "Expected base class name");
        superclass_name = base.lexeme;
        consume(TokenType::RPAREN, "Expected ')' after base class name");
    }
    
    consume(TokenType::COLON, "Expected ':' after class header");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indented block for class body");
    
    // Parse class body as a block and collect methods
    auto body_statements = block();
    std::vector<std::unique_ptr<FunctionDecl>> methods;
    
    for (auto& stmt : body_statements) {
        if (auto* fn = dynamic_cast<FunctionDecl*>(stmt.get())) {
            // Transfer ownership into methods vector
            std::unique_ptr<FunctionDecl> method(fn);
            stmt.release();
            methods.push_back(std::move(method));
        }
        // Non-function statements in class body are ignored for now
    }
    
    return std::make_unique<ClassDecl>(name.lexeme, superclass_name, std::move(methods));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::TRY})) return tryStatement();
    if (match({TokenType::THROW})) return throwStatement();
    
    return exprStatement();
}

std::unique_ptr<Stmt> Parser::exprStatement() {
    auto expr = expression();
    skipNewlines();
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    std::unique_ptr<Expr> value = nullptr;
    
    if (!check(TokenType::NEWLINE) && !isAtEnd()) {
        value = expression();
    }
    
    skipNewlines();
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    auto condition = expression();
    consume(TokenType::COLON, "Expected ':' after if condition");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indented block");
    
    auto then_branch = block();
    
    std::vector<std::unique_ptr<Stmt>> else_branch;
    if (match({TokenType::ELSE})) {
        consume(TokenType::COLON, "Expected ':' after else");
        skipNewlines();
        consume(TokenType::INDENT, "Expected indented block");
        else_branch = block();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch),
                                    std::move(else_branch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    auto condition = expression();
    consume(TokenType::COLON, "Expected ':' after while condition");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indented block");
    
    auto body = block();
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement() {
    Token var = consumeToken(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::IN, "Expected 'in' in for loop");
    auto iterable = expression();
    consume(TokenType::COLON, "Expected ':' after for clause");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indented block");
    
    auto body = block();
    
    return std::make_unique<ForStmt>(var.lexeme, std::move(iterable), std::move(body));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!check(TokenType::DEDENT) && !isAtEnd()) {
        skipNewlines();
        if (!check(TokenType::DEDENT) && !isAtEnd()) {
            statements.push_back(declaration());
        }
    }
    
    consume(TokenType::DEDENT, "Expected dedent");
    return statements;
}

// Expression parsing

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = logicalOr();
    
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        auto value = assignment();
        
        // Check if left side is a variable or property
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(var->name, std::move(value));
        } else if (auto* get = dynamic_cast<GetExpr*>(expr.get())) {
            // Transform "object.name = value" into a SetExpr
            auto object = std::move(get->object);
            std::string name = get->name;
            return std::make_unique<SetExpr>(std::move(object), name, std::move(value));
        }
        
        throw error(equals, "Invalid assignment target");
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::logicalOr() {
    auto expr = logicalAnd();
    
    while (match({TokenType::OR})) {
        std::string op = previous().lexeme;
        auto right = logicalAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::logicalAnd() {
    auto expr = equality();
    
    while (match({TokenType::AND})) {
        std::string op = previous().lexeme;
        auto right = equality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    
    while (match({TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL,
                  TokenType::LESS, TokenType::LESS_EQUAL})) {
        std::string op = previous().lexeme;
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = unary();
    
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        std::string op = previous().lexeme;
        auto right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        std::string op = previous().lexeme;
        auto right = unary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    
    return power();
}

std::unique_ptr<Expr> Parser::power() {
    auto expr = call();
    
    if (match({TokenType::POWER})) {
        auto right = power();
        return std::make_unique<BinaryExpr>(std::move(expr), "**", std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match({TokenType::LPAREN})) {
            std::vector<std::unique_ptr<Expr>> arguments;
            
            if (!check(TokenType::RPAREN)) {
                do {
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RPAREN, "Expected ')' after arguments");
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(arguments));
        } else if (match({TokenType::LBRACKET})) {
            auto index = expression();
            consume(TokenType::RBRACKET, "Expected ']' after index");
            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index));
        } else if (match({TokenType::DOT})) {
            Token name = consumeToken(TokenType::IDENTIFIER, "Expected property name after '.'");
            expr = std::make_unique<GetExpr>(std::move(expr), name.lexeme);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::TRUE})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::BOOLEAN, "true");
    }
    if (match({TokenType::FALSE})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::BOOLEAN, "false");
    }
    if (match({TokenType::NONE})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::NONE, "none");
    }
    
    if (match({TokenType::INTEGER})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::INTEGER, previous().lexeme);
    }
    if (match({TokenType::FLOAT})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::FLOAT, previous().lexeme);
    }
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(LiteralExpr::Type::STRING, previous().lexeme);
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }
    
    if (match({TokenType::LBRACKET})) {
        std::vector<std::unique_ptr<Expr>> elements;
        
        if (!check(TokenType::RBRACKET)) {
            do {
                elements.push_back(expression());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RBRACKET, "Expected ']' after list elements");
        return std::make_unique<ListExpr>(std::move(elements));
    }
    
    if (match({TokenType::LPAREN})) {
        auto expr = expression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw error(peek(), "Expected expression");
}

std::unique_ptr<Stmt> Parser::tryStatement() {
    // try:
    consume(TokenType::COLON, "Expected ':' after 'try'");
    skipNewlines();
    consume(TokenType::INDENT, "Expected indentation after 'try:'");
    
    // Parse try body
    std::vector<std::unique_ptr<Stmt>> try_body;
    while (!check(TokenType::DEDENT) && !isAtEnd()) {
        try_body.push_back(declaration());
        skipNewlines();
    }
    consume(TokenType::DEDENT, "Expected dedent after try block");
    
    // Parse catch clauses
    std::vector<CatchClause> catch_clauses;
    while (match({TokenType::CATCH})) {
        std::string exception_type;
        std::string variable_name;
        
        // Check for exception type and variable binding
        if (check(TokenType::IDENTIFIER)) {
            Token first = advance();
            
            if (match({TokenType::AS})) {
                // catch ExceptionType as e:
                exception_type = first.lexeme;
                Token var_token = consumeToken(TokenType::IDENTIFIER, "Expected variable name after 'as'");
                variable_name = var_token.lexeme;
            } else {
                // catch e:
                variable_name = first.lexeme;
            }
        }
        
        consume(TokenType::COLON, "Expected ':' after catch clause");
        skipNewlines();
        consume(TokenType::INDENT, "Expected indentation after catch:");
        
        std::vector<std::unique_ptr<Stmt>> catch_body;
        while (!check(TokenType::DEDENT) && !isAtEnd()) {
            catch_body.push_back(declaration());
            skipNewlines();
        }
        consume(TokenType::DEDENT, "Expected dedent after catch block");
        
        catch_clauses.emplace_back(exception_type, variable_name, std::move(catch_body));
    }
    
    // Parse finally clause
    std::vector<std::unique_ptr<Stmt>> finally_body;
    if (match({TokenType::FINALLY})) {
        consume(TokenType::COLON, "Expected ':' after 'finally'");
        skipNewlines();
        consume(TokenType::INDENT, "Expected indentation after finally:");
        
        while (!check(TokenType::DEDENT) && !isAtEnd()) {
            finally_body.push_back(declaration());
            skipNewlines();
        }
        consume(TokenType::DEDENT, "Expected dedent after finally block");
    }
    
    // Validate: must have at least one catch or finally
    if (catch_clauses.empty() && finally_body.empty()) {
        throw error(previous(), "Try statement must have at least one catch or finally clause");
    }
    
    return std::make_unique<TryStmt>(std::move(try_body),
                                     std::move(catch_clauses),
                                     std::move(finally_body));
}

std::unique_ptr<Stmt> Parser::throwStatement() {
    std::string exception_type = "RuntimeError";  // Default
    std::unique_ptr<Expr> message;
    
    // Check if exception type is specified: throw ValueError("message")
    if (check(TokenType::IDENTIFIER) && peek().lexeme[0] >= 'A' && peek().lexeme[0] <= 'Z') {
        // Looks like an exception type (starts with capital letter)
        Token type_token = advance();
        
        if (match({TokenType::LPAREN})) {
            exception_type = type_token.lexeme;
            message = expression();
            consume(TokenType::RPAREN, "Expected ')' after exception message");
        } else {
            // Not a function call, treat as regular expression
            current--;  // Back up
            message = expression();
        }
    } else {
        // throw "message"
        message = expression();
    }
    
    skipNewlines();
    return std::make_unique<ThrowStmt>(exception_type, std::move(message));
}

std::unique_ptr<Stmt> Parser::importDeclaration() {
    // import module
    // import module as alias
    // from module import name1, name2
    // from module import name1 as alias1, name2
    
    // Check if FROM was already matched (previous token is FROM)
    bool is_from_import = previous().type == TokenType::FROM;
    
    if (is_from_import) {
        // from module import name1, name2
        Token module_token = consumeToken(TokenType::IDENTIFIER, "Expected module name after 'from'");
        std::string module_name = module_token.lexeme;
        
        consume(TokenType::IMPORT, "Expected 'import' after module name");
        
        std::vector<std::string> imported_names;
        
        // Parse imported names
        do {
            Token name_token = consumeToken(TokenType::IDENTIFIER, "Expected identifier in import list");
            std::string name = name_token.lexeme;
            
            // Check for alias
            if (match({TokenType::AS})) {
                Token alias_token = consumeToken(TokenType::IDENTIFIER, "Expected alias name after 'as'");
                name = alias_token.lexeme; // For now, just use the alias as the name
            }
            
            imported_names.push_back(name);
        } while (match({TokenType::COMMA}));
        
        skipNewlines();
        return std::unique_ptr<ImportStmt>(new ImportStmt(module_name, imported_names));
    } else {
        // import module
        // import module as alias
        Token module_token = consumeToken(TokenType::IDENTIFIER, "Expected module name after 'import'");
        std::string module_name = module_token.lexeme;
        std::string alias;
        
        // Check for alias
        if (match({TokenType::AS})) {
            Token alias_token = consumeToken(TokenType::IDENTIFIER, "Expected alias name after 'as'");
            alias = alias_token.lexeme;
        }
        
        skipNewlines();
        return std::unique_ptr<ImportStmt>(new ImportStmt(module_name, alias));
    }
}

} // namespace sapphire