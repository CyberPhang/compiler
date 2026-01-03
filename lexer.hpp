#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

enum class TokenType {
    TOKEN_IDENTIFIER,
    
    TOKEN_CONSTANT,

    // Keywords
    TOKEN_INT,
    TOKEN_VOID,
    TOKEN_RET,


    // Parans and braces
    TOKEN_OPEN_PARAN,
    TOKEN_CLOSED_PARAN,
    TOKEN_OPEN_BRACE,
    TOKEN_CLOSED_BRACE,
    TOKEN_SEMI,

    // Unary operators
    TOKEN_TILDE,
    TOKEN_NEG,
    TOKEN_DEC,

    TOKEN_EOF,
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int col;

    Token(TokenType type, std::string lexeme, int line, int col);
};

class Lexer {
    private:
        int start;
        int curr;
        int line;
        int col;

        std::vector<Token> tokens;
        static inline const std::unordered_map<std::string, TokenType> keywords = {
            {"int", TokenType::TOKEN_INT},
            {"void", TokenType::TOKEN_VOID},
            {"return", TokenType::TOKEN_RET}
        };

        void add_token(TokenType token, std::string_view lexeme, int line, int col);
        void add_next_token(std::string_view input);

    public:
        Lexer();
        const std::vector<Token>& get_tokens();
        const std::vector<Token>& read(std::string_view input);
};

#endif