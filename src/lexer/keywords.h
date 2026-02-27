#ifndef SAPPHIRE_KEYWORDS_H
#define SAPPHIRE_KEYWORDS_H

#include "token.h"
#include <map>
#include <string>

namespace sapphire {

static const std::map<std::string, TokenType> KEYWORDS = {
    {"let", TokenType::LET},
    {"const", TokenType::CONST},
    {"fn", TokenType::FN},
    {"return", TokenType::RETURN},
    {"if", TokenType::IF},
    {"elif", TokenType::ELIF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"in", TokenType::IN},
    {"match", TokenType::MATCH},
    {"class", TokenType::CLASS},
    {"import", TokenType::IMPORT},
    {"from", TokenType::FROM},
    {"as", TokenType::AS},
    {"true", TokenType::TRUE},
    {"True", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"False", TokenType::FALSE},
    {"none", TokenType::NONE},
    {"None", TokenType::NONE},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
    {"pass", TokenType::PASS},
    {"async", TokenType::ASYNC},
    {"await", TokenType::AWAIT},
    {"try", TokenType::TRY},
    {"catch", TokenType::CATCH},
    {"finally", TokenType::FINALLY},
    {"throw", TokenType::THROW}
};

} // namespace sapphire

#endif // SAPPHIRE_KEYWORDS_H
