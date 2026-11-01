#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>

// Represents all possible tokens that we can read in from a file
enum class TokenType {
    IDENTIFIER,
    
    CONSTANT,

    // Keywords
    INT,
    VOID,
    RET,

    // Parans and braces
    OPEN_PARAN,
    CLOSED_PARAN,
    OPEN_BRACE,
    CLOSED_BRACE,
    SEMI,

    // Unary operators
    TILDE,
    NEG, // also subtraction token
    DEC,

    // Binary Operators
    ADD,
    MUL,
    DIV,
    MOD,

    END_OF_FILE
};

// Overload for std::ostream<< so that TokenTypes can be printed to std::ostreams
std::ostream& operator<<(std::ostream& os, TokenType tt);

// Represents a single token, which consists of some type (as defined
// in the enum TokenType) and a lexem representing the characters that
// comprise it. Also contains location information (i.e. where the 
// token was found in the file)
struct Token {
    TokenType type_;
    std::string lexeme_;
    int line_;
    int col_;

    // Creates a token of the given type with the given lexeme
    // at the given line and column numbers
    Token(TokenType type, std::string lexeme, int line, int col) : 
        type_(type), lexeme_(std::move(lexeme)), line_(line), col_(col) {}

    // Default constructor for Tokens. Generates a token representing an EOF
    // character as the first character in the file.
    Token() : type_(TokenType::END_OF_FILE), lexeme_(""), line_(0), col_(0) {}

    // Defines fieldwise == for Token. This is important for testing.
    bool operator==(const Token&) const = default;
};

// Overload std::ostream << so that Tokens can be printed to std::ostream
std::ostream& operator<<(std::ostream& os, Token t);

// Represents a lexer, which takes in some code as string input and spits out
// Tokens corresponding to what it just read, one at a time.
class Lexer {
    private:
        int curr_;
        int line_;
        int col_;
        std::string content_;

        static inline const std::unordered_map<std::string, TokenType> keywords = {
            {"int", TokenType::INT},
            {"void", TokenType::VOID},
            {"return", TokenType::RET}
        };

    public:
        // Creates a lexer containing the given file content as a string. The content
        // will be read as C code and parsed into tokens.
        // Args:
        // - std::string content - the raw text of the code, as an std::string
        Lexer(std::string content) : curr_(0), line_(0), col_(0), content_(std::move(content)) {}
        
        // Returns the next token from content_.
        // Returns:
        // - Token - the next Token to lex in content_. 
        Token get_next_token();
        
};

#endif