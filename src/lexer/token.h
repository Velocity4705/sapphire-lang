#ifndef SAPPHIRE_TOKEN_H
#define SAPPHIRE_TOKEN_H

#include <string>
#include <ostream>

namespace sapphire {

enum class TokenType {
    // Literals
    INTEGER,
    FLOAT,
    STRING,
    IDENTIFIER,
    
    // Keywords
    LET,
    CONST,
    FN,
    RETURN,
    IF,
    ELIF,
    ELSE,
    FOR,
    WHILE,
    IN,
    MATCH,
    CLASS,
    IMPORT,
    FROM,
    AS,
    TRUE,
    FALSE,
    NONE,
    AND,
    OR,
    NOT,
    BREAK,
    CONTINUE,
    PASS,
    ASYNC,
    AWAIT,
    TRY,
    CATCH,
    FINALLY,
    THROW,
    
    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    POWER,
    EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    
    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COLON,
    COMMA,
    DOT,
    ARROW,
    QUESTION,
    
    // Special
    NEWLINE,
    INDENT,
    DEDENT,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t, const std::string& lex, int ln, int col)
        : type(t), lexeme(lex), line(ln), column(col) {}
};

std::string tokenTypeToString(TokenType type);
std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace sapphire

#endif // SAPPHIRE_TOKEN_H
