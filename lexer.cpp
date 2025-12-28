#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>
#include "lexer.hpp"

Token::Token(TokenType type, std::string lexeme, int line, int col) : 
    type(type), lexeme(std::move(lexeme)), line(line), col(col) {}

void Lexer::add_token(TokenType token, std::string_view lexeme, int line, int col) {
    tokens.emplace_back(token, std::string(lexeme), line, col);
}

void Lexer::add_next_token(std::string_view input) {
    int last_char { input[curr] };
    while (curr < input.length() && std::isspace(last_char)) {
        if (last_char == '\n') {
            ++line;
            col = 0;
        } else {
            ++col;
        }
        ++curr;
        last_char = input[curr];
    }

    start = curr;

    if (curr == input.length()) {
        add_token(TokenType::TOKEN_EOF, "", line, col);
        return;
    }

    switch (last_char) {
        case '(':
            add_token(TokenType::TOKEN_OPEN_PARAN, "(", line, col);
            break;
        case ')':
            add_token(TokenType::TOKEN_CLOSED_PARAN, ")", line, col);
            break;
        case '{':
            add_token(TokenType::TOKEN_OPEN_BRACE, "{", line, col);
            break;
        case '}':
            add_token(TokenType::TOKEN_CLOSED_BRACE, "}", line, col);
            break;
        case ';':
            add_token(TokenType::TOKEN_SEMI, ";", line, col);
            break;
        default:
            if (std::isdigit(last_char)) {
                int value {};

                while (curr < input.length() && std::isdigit(input[curr])) {
                    value = value * 10 + (input[curr] - '0');
                    ++curr;
                    ++col;
                }

                std::string value_s = std::to_string(value);

                add_token(TokenType::TOKEN_CONSTANT, value_s, line, col);

                if (curr >= input.length()) {
                    add_token(TokenType::TOKEN_EOF, "", line, col);
                }
                col += value_s.length() - 1;
                --curr;
            } else if (std::isalpha(last_char) || last_char == '_') {
                start = curr;
                while (curr < input.length() && (std::isalnum(input[curr]) || input[curr] == '_')) {
                    ++curr;
                }

                std::string name = std::string(input.substr(start, curr - start));
                if (keywords.count(name)) {
                    add_token(keywords.at(name), name, line, col);
                } else {
                    add_token(TokenType::TOKEN_IDENTIFIER, name, line, col);
                }
                col += name.length() - 1;
                --curr;
            }
    }
    ++col;
    ++curr;
}

Lexer::Lexer() : curr(0), start(0), line(0), col(0) {}

const std::vector<Token>& Lexer::get_tokens() {
    return tokens;
}

const std::vector<Token>& Lexer::read(std::string_view input) {
    while (curr < input.length()) {
        start = curr;
        add_next_token(input);
    }

    if (tokens.back().type != TokenType::TOKEN_EOF) {
        add_token(TokenType::TOKEN_EOF, "", line, col);
    }

    return tokens;
}