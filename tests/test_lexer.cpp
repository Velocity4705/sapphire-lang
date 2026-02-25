#include "../src/lexer/lexer.h"
#include <cassert>
#include <iostream>

using namespace sapphire;

void test_integers() {
    Lexer lexer("42 123 0");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::INTEGER);
    assert(tokens[0].lexeme == "42");
    assert(tokens[1].type == TokenType::INTEGER);
    assert(tokens[1].lexeme == "123");
    assert(tokens[2].type == TokenType::INTEGER);
    assert(tokens[2].lexeme == "0");
    
    std::cout << "✓ test_integers passed\n";
}

void test_floats() {
    Lexer lexer("3.14 0.5 2.0");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::FLOAT);
    assert(tokens[0].lexeme == "3.14");
    assert(tokens[1].type == TokenType::FLOAT);
    assert(tokens[1].lexeme == "0.5");
    
    std::cout << "✓ test_floats passed\n";
}

void test_strings() {
    Lexer lexer("\"hello\" 'world'");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::STRING);
    assert(tokens[0].lexeme == "hello");
    assert(tokens[1].type == TokenType::STRING);
    assert(tokens[1].lexeme == "world");
    
    std::cout << "✓ test_strings passed\n";
}

void test_keywords() {
    Lexer lexer("let fn if else while for");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::FN);
    assert(tokens[2].type == TokenType::IF);
    assert(tokens[3].type == TokenType::ELSE);
    assert(tokens[4].type == TokenType::WHILE);
    assert(tokens[5].type == TokenType::FOR);
    
    std::cout << "✓ test_keywords passed\n";
}

void test_operators() {
    Lexer lexer("+ - * / == != < > <= >=");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::PLUS);
    assert(tokens[1].type == TokenType::MINUS);
    assert(tokens[2].type == TokenType::STAR);
    assert(tokens[3].type == TokenType::SLASH);
    assert(tokens[4].type == TokenType::EQUAL_EQUAL);
    assert(tokens[5].type == TokenType::NOT_EQUAL);
    assert(tokens[6].type == TokenType::LESS);
    assert(tokens[7].type == TokenType::GREATER);
    assert(tokens[8].type == TokenType::LESS_EQUAL);
    assert(tokens[9].type == TokenType::GREATER_EQUAL);
    
    std::cout << "✓ test_operators passed\n";
}

void test_identifiers() {
    Lexer lexer("x foo bar_baz _private");
    auto tokens = lexer.tokenize();
    
    assert(tokens[0].type == TokenType::IDENTIFIER);
    assert(tokens[0].lexeme == "x");
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "foo");
    assert(tokens[2].type == TokenType::IDENTIFIER);
    assert(tokens[2].lexeme == "bar_baz");
    
    std::cout << "✓ test_identifiers passed\n";
}

int main() {
    std::cout << "Running Lexer Tests...\n\n";
    
    test_integers();
    test_floats();
    test_strings();
    test_keywords();
    test_operators();
    test_identifiers();
    
    std::cout << "\n✅ All lexer tests passed!\n";
    return 0;
}
