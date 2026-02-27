#include "token.h"
#include <map>

namespace sapphire {

std::string tokenTypeToString(TokenType type) {
    static const std::map<TokenType, std::string> typeNames = {
        // Literals
        {TokenType::INTEGER, "INTEGER"},
        {TokenType::FLOAT, "FLOAT"},
        {TokenType::STRING, "STRING"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        
        // Keywords
        {TokenType::LET, "LET"},
        {TokenType::CONST, "CONST"},
        {TokenType::FN, "FN"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::IF, "IF"},
        {TokenType::ELIF, "ELIF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::FOR, "FOR"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::IN, "IN"},
        {TokenType::MATCH, "MATCH"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::NONE, "NONE"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"},
        {TokenType::BREAK, "BREAK"},
        {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::PASS, "PASS"},
        {TokenType::ASYNC, "ASYNC"},
        {TokenType::AWAIT, "AWAIT"},
        {TokenType::TRY, "TRY"},
        {TokenType::CATCH, "CATCH"},
        {TokenType::FINALLY, "FINALLY"},
        {TokenType::THROW, "THROW"},
        
        // Operators
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::POWER, "POWER"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
        {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
        
        // Delimiters
        {TokenType::LPAREN, "LPAREN"},
        {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACE, "LBRACE"},
        {TokenType::RBRACE, "RBRACE"},
        {TokenType::LBRACKET, "LBRACKET"},
        {TokenType::RBRACKET, "RBRACKET"},
        {TokenType::COLON, "COLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::ARROW, "ARROW"},
        {TokenType::QUESTION, "QUESTION"},
        
        // Special
        {TokenType::NEWLINE, "NEWLINE"},
        {TokenType::INDENT, "INDENT"},
        {TokenType::DEDENT, "DEDENT"},
        {TokenType::END_OF_FILE, "END_OF_FILE"},
        {TokenType::INVALID, "INVALID"}
    };
    
    auto it = typeNames.find(type);
    if (it != typeNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << tokenTypeToString(token.type) << "('" << token.lexeme << "')";
    return os;
}

} // namespace sapphire
