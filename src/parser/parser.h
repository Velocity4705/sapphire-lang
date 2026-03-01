#ifndef SAPPHIRE_PARSER_H
#define SAPPHIRE_PARSER_H

#include "../lexer/token.h"
#include "stmt.h"
#include "expr.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace sapphire {

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek() const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    void consume(TokenType type, const std::string& message);
    Token consumeToken(TokenType type, const std::string& message);
    void skipNewlines();
    
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
    
    // Statement parsing
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> functionDeclaration();
    std::unique_ptr<Stmt> classDeclaration();
    std::unique_ptr<Stmt> importDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> exprStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> forStatement();
    std::unique_ptr<Stmt> tryStatement();
    std::unique_ptr<Stmt> throwStatement();
    std::vector<std::unique_ptr<Stmt>> block();
    
    // Expression parsing
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> assignment();
    std::unique_ptr<Expr> logicalOr();
    std::unique_ptr<Expr> logicalAnd();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> unary();
    std::unique_ptr<Expr> power();
    std::unique_ptr<Expr> call();
    std::unique_ptr<Expr> primary();

public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
};

} // namespace sapphire

#endif // SAPPHIRE_PARSER_H
