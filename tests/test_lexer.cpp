#include <string>
#include <cstdlib>
#include <iostream>
#include "../include/lexer.hpp"
#include "test_utils.hpp"

// General helper function to test whether lexer is outputting the expected Tokens as it
// reads through code.
// Args:
// - std::string code - the actual code to lex, represented as a raw string
// - std::vector<Token> expected - the expected outcome of Tokens from the lexer.
// Returns:
// - true if and only if the expected Token stream matches the actual Token stream.
static bool test_lex_gnt(std::string code, std::vector<Token> expected) {
    Lexer lexer = Lexer(code);
    std::vector<Token> token_stream;
    while (token_stream.size() == 0 || token_stream.back().type_ != TokenType::END_OF_FILE) {
        token_stream.emplace_back(lexer.get_next_token());
    }
    TTC_ASSERT_EQ(token_stream, expected);
}

// Test whether or not the lexer can properly lex reserved keywords.
// Returns true if and only if successful lexing.
static bool test_lex_keywords() {
    std::string code = "int void return";
    std::vector<Token> expected;
    expected.emplace_back(TokenType::INT, "int", 0, 0);
    expected.emplace_back(TokenType::VOID, "void", 0, 4);
    expected.emplace_back(TokenType::RET, "return", 0, 9);
    expected.emplace_back(TokenType::END_OF_FILE, "", 0, 15);
    return test_lex_gnt(code, expected);
}

// Test whehther or not the lexer can properly lex unary and binary operators.
// Returns true if and only if successful.
static bool test_lex_operators() {
    std::string code = "+-/%--~*";
    std::vector<Token> expected;
    expected.emplace_back(TokenType::ADD, "+", 0, 0);
    expected.emplace_back(TokenType::NEG, "-", 0, 1);
    expected.emplace_back(TokenType::DIV, "/", 0, 2);
    expected.emplace_back(TokenType::MOD, "%", 0, 3);
    expected.emplace_back(TokenType::DEC, "--", 0, 4);
    expected.emplace_back(TokenType::TILDE, "~", 0, 6);
    expected.emplace_back(TokenType::MUL, "*", 0, 7);
    expected.emplace_back(TokenType::END_OF_FILE, "", 0, 8);
    return test_lex_gnt(code, expected);
}

// Test whether or not the lexer can properly lex scoping characters. These include
// braces, parantheses, and semicolons since braces. Broadly speaking, "scoping characters"
// informally refer to special characters used throughout C syntax to define scope,
// delimit parts of code, etc.
static bool test_lex_scope() {
    std::string code = ";(){}";
    std::vector<Token> expected;
    expected.emplace_back(TokenType::SEMI, ";", 0, 0);
    expected.emplace_back(TokenType::OPEN_PARAN, "(", 0, 1);
    expected.emplace_back(TokenType::CLOSED_PARAN, ")", 0, 2);
    expected.emplace_back(TokenType::OPEN_BRACE, "{", 0, 3);
    expected.emplace_back(TokenType::CLOSED_BRACE, "}", 0, 4);
    expected.emplace_back(TokenType::END_OF_FILE, "", 0, 5);
    return test_lex_gnt(code, expected);
}

// Test whether or not the lexer can properly lex identifiers. These could potentially
// be variable, function, or struct names. Returns true if and only if lexed successfully.
static bool test_lex_identifiers() {
    std::string code = "abcd_\nabc_123";
    std::vector<Token> expected;
    expected.emplace_back(TokenType::IDENTIFIER, "abcd_", 0, 0);
    expected.emplace_back(TokenType::IDENTIFIER, "abc_123", 1, 0);
    expected.emplace_back(TokenType::END_OF_FILE, "", 1, 7);
    return test_lex_gnt(code, expected);
}

// Test whether or not the lexer can properly lex constants, as well as whether or
// not it can distinguish between constants and identifiers.
static bool test_lex_constants() {
    std::string code = "12345\n123abc12";
    std::vector<Token> expected;
    expected.emplace_back(TokenType::CONSTANT, "12345", 0, 0);
    expected.emplace_back(TokenType::CONSTANT, "123", 1, 0);
    expected.emplace_back(TokenType::IDENTIFIER, "abc12", 1, 3);
    expected.emplace_back(TokenType::END_OF_FILE, "", 1, 8);
    return test_lex_gnt(code, expected);
}

static bool test_lex_program() {
    std::string code = std::string(
        "int main(void) {\n"
        "\treturn ~(-2);\n"        
        "}"
    );
    std::vector<Token> expected;
    expected.emplace_back(TokenType::INT, "int", 0, 0);
    expected.emplace_back(TokenType::IDENTIFIER, "main", 0, 4);
    expected.emplace_back(TokenType::OPEN_PARAN, "(", 0, 8);
    expected.emplace_back(TokenType::VOID, "void", 0, 9);
    expected.emplace_back(TokenType::CLOSED_PARAN, ")", 0, 13);
    expected.emplace_back(TokenType::OPEN_BRACE, "{", 0, 15);
    expected.emplace_back(TokenType::RET, "return", 1, 1);
    expected.emplace_back(TokenType::TILDE, "~", 1, 8);
    expected.emplace_back(TokenType::OPEN_PARAN, "(", 1, 9);
    expected.emplace_back(TokenType::NEG, "-", 1, 10);
    expected.emplace_back(TokenType::CONSTANT, "2", 1, 11);
    expected.emplace_back(TokenType::CLOSED_PARAN, ")", 1, 12);
    expected.emplace_back(TokenType::SEMI, ";", 1, 13);
    expected.emplace_back(TokenType::CLOSED_BRACE, "}", 2, 0);
    expected.emplace_back(TokenType::END_OF_FILE, "", 2, 1);
    return test_lex_gnt(code, expected);
}

int main() {
    int num_failed = 0;
    num_failed += !test_lex_keywords();
    num_failed += !test_lex_operators();
    num_failed += !test_lex_scope();
    num_failed += !test_lex_identifiers();
    num_failed += !test_lex_constants();
    num_failed += !test_lex_program();
    std::cout << "Tests failed: " << num_failed << "\n";
    return EXIT_SUCCESS;
}