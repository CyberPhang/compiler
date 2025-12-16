#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>
#include "lexer.hpp"

Token::Token(TokenType type, std::string lexeme) : type(type), lexeme(std::move(lexeme)) {}

void Lexer::add_token(TokenType token, std::string_view lexeme) {
    tokens.push_back(std::make_unique<Token>(token, std::string(lexeme)));
}

void Lexer::add_next_token(std::string_view input) {
    int last_char { input[curr] };
    while (curr < input.length() && std::isspace(last_char)) {
        curr++;
        last_char = input[curr];
    }

    start = curr;

    if (curr == input.length()) {
        add_token(TOKEN_EOF, "");
        return;
    }

    switch (last_char) {
        case '(':
            add_token(TOKEN_OPEN_PARAN, "(");
            break;
        case ')':
            add_token(TOKEN_OPEN_PARAN, ")");
            break;
        case '{':
            add_token(TOKEN_OPEN_BRACE, "{");
            break;
        case '}':
            add_token(TOKEN_CLOSED_BRACE, "}");
            break;
        case ';':
            add_token(TOKEN_SEMI, ";");
            break;
        default:
            if (std::isdigit(last_char)) {
                int value {};

                while (curr < input.length() && std::isdigit(last_char)) {
                    value += last_char - '0';
                    value *= 10;
                    curr++;
                    last_char = input[curr];
                }
                curr--;
                value /= 10;

                add_token(TOKEN_CONSTANT, std::to_string(value));

                if (curr == input.length()) {
                    add_token(TOKEN_EOF, "");
                }
            } else if (std::isalpha(last_char) || last_char == '_') {
                start = curr;
                while (curr < input.length() && (std::isalnum(last_char) || last_char == '_')) {
                    last_char = input[curr];
                    curr++;
                }
                curr--;

                std::string name = std::string(input.substr(start, curr - start));
                if (keywords.count(name)) {
                    add_token(keywords.at(name), name);
                } else {
                    add_token(TOKEN_IDENTIFIER, name);
                }
                curr--;
            }
    }
    curr++;
}

Lexer::Lexer() {
    curr = start = 0;
}

const std::vector<std::unique_ptr<Token>>& Lexer::get_tokens() {
    return tokens;
}

const std::vector<std::unique_ptr<Token>>& Lexer::read(std::string_view input) {
    while (curr < input.length()) {
        start = curr;
        add_next_token(input);
    }

    return tokens;
}