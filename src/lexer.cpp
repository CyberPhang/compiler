#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string_view>
#include <iostream>
#include "../include/lexer.hpp"

static const std::unordered_map<TokenType, std::string_view> token_names = {
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::CONSTANT, "CONSTANT"},
    {TokenType::INT, "INT"},
    {TokenType::VOID, "VOID"},
    {TokenType::RET, "RET"},
    {TokenType::OPEN_PARAN, "OPEN_PARAN"},
    {TokenType::CLOSED_PARAN, "CLOSED_PARAN"},
    {TokenType::OPEN_BRACE, "OPEN_BRACE"},
    {TokenType::CLOSED_BRACE, "CLOSED_BRACE"},
    {TokenType::SEMI, "SEMI"},
    {TokenType::TILDE, "TILDE"},
    {TokenType::NEG, "NEG"},
    {TokenType::DEC, "DEC"},
    {TokenType::ADD, "ADD"},
    {TokenType::MUL, "MUL"},
    {TokenType::DIV, "DIV"},
    {TokenType::MOD, "MOD"},
    {TokenType::END_OF_FILE, "EOF"}
};

std::ostream& operator<<(std::ostream& os, TokenType tt) {
    os << token_names.at(tt);
    return os;
}

std::ostream& operator<<(std::ostream& os, Token t) {
    os << "{" << "type: " << t.type_ << ", lexeme: " << t.lexeme_ << ", line: " << t.line_
       << ", col: " << t.col_ << "}";
    return os;
}

// void Lexer::add_token(TokenType token, std::string_view lexeme, int line, int col) {
//     tokens.emplace_back(token, std::string(lexeme), line, col);
// }

// void Lexer::add_next_token(const std::string_view input) {
//     int last_char { input[curr] };
//     while (curr < input.length() && std::isspace(last_char)) {
//         if (last_char == '\n') {
//             ++line;
//             col = 0;
//         } else {
//             ++col;
//         }
//         ++curr;
//         last_char = input[curr];
//     }

//     start = curr;

//     if (curr == input.length()) {
//         add_token(TokenType::TOKEN_EOF, "", line, col);
//         return;
//     }

//     switch (last_char) {
//         case '(':
//             add_token(TokenType::TOKEN_OPEN_PARAN, "(", line, col);
//             break;
//         case ')':
//             add_token(TokenType::TOKEN_CLOSED_PARAN, ")", line, col);
//             break;
//         case '{':
//             add_token(TokenType::TOKEN_OPEN_BRACE, "{", line, col);
//             break;
//         case '}':
//             add_token(TokenType::TOKEN_CLOSED_BRACE, "}", line, col);
//             break;
//         case ';':
//             add_token(TokenType::TOKEN_SEMI, ";", line, col);
//             break;
//         case '~':
//             add_token(TokenType::TOKEN_TILDE, "~", line, col);
//             break;
//         case '-':
//             if (curr + 1 < input.length() && input[curr + 1] == '-') {
//                 add_token(TokenType::TOKEN_DEC, "--", line, col);
//                 ++curr;
//             } else {
//                 add_token(TokenType::TOKEN_NEG, "-", line, col);
//             }
//             break;
//         case '+':
//             add_token(TokenType::TOKEN_ADD, "+", line, col);
//             break;
//         case '*':
//             add_token(TokenType::TOKEN_MUL, "*", line, col);
//             break;
//         case '/':
//             add_token(TokenType::TOKEN_DIV, "/", line, col);
//             break;
//         case '%':
//             add_token(TokenType::TOKEN_MOD, "%", line, col);
//             break;
//         default:
//             if (std::isdigit(last_char)) {
//                 int value {};

//                 while (curr < input.length() && std::isdigit(input[curr])) {
//                     value = value * 10 + (input[curr] - '0');
//                     ++curr;
//                     ++col;
//                 }

//                 std::string value_s = std::to_string(value);

//                 add_token(TokenType::TOKEN_CONSTANT, value_s, line, col);

//                 if (curr >= input.length()) {
//                     add_token(TokenType::TOKEN_EOF, "", line, col);
//                 }
//                 col += value_s.length() - 1;
//                 --curr;
//             } else if (std::isalpha(last_char) || last_char == '_') {
//                 start = curr;
//                 while (curr < input.length() && (std::isalnum(input[curr]) || input[curr] == '_')) {
//                     ++curr;
//                 }

//                 std::string name = std::string(input.substr(start, curr - start));
//                 if (keywords.count(name)) {
//                     add_token(keywords.at(name), name, line, col);
//                 } else {
//                     add_token(TokenType::TOKEN_IDENTIFIER, name, line, col);
//                 }
//                 col += name.length() - 1;
//                 --curr;
//             }
//     }
//     ++col;
//     ++curr;
// }

// Lexer::Lexer() : curr(0), start(0), line(0), col(0) {}

// const std::vector<Token>& Lexer::get_tokens() {
//     return tokens;
// }

// const std::vector<Token>& Lexer::read(std::string_view input) {
//     while (curr < input.length()) {
//         start = curr;
//         add_next_token(input);
//     }

//     if (tokens.back().type != TokenType::TOKEN_EOF) {
//         add_token(TokenType::TOKEN_EOF, "", line, col);
//     }

//     return tokens;
// }


Token Lexer::get_next_token() {
    char last_char { content_[curr_] };
    while (curr_ < content_.length() && std::isspace(last_char)) {
        if (last_char == '\n') {
            ++line_;
            col_ = 0;
        } else {
            ++col_;
        }
        ++curr_;
        last_char = content_[curr_];
    }

    int start = curr_;
    if (curr_ == content_.length()) {
        return Token(TokenType::END_OF_FILE, "", line_, col_);
    }

    Token result;
    switch (last_char) {
        case '(':
            result = Token(TokenType::OPEN_PARAN, "(", line_, col_);
            break;
        case ')':
            result = Token(TokenType::CLOSED_PARAN, ")", line_, col_);
            break;
        case '{':
            result = Token(TokenType::OPEN_BRACE, "{", line_, col_);
            break;
        case '}':
            result = Token(TokenType::CLOSED_BRACE, "}", line_, col_);
            break;
        case ';':
            result = Token(TokenType::SEMI, ";", line_, col_);
            break;
        case '~':
            result = Token(TokenType::TILDE, "~", line_, col_);
            break;
        case '-':
            if (curr_ + 1 < content_.length() && content_[curr_ + 1] == '-') {
                result = Token(TokenType::DEC, "--", line_, col_);
                ++curr_;
                ++col_;
            } else {
                result = Token(TokenType::NEG, "-", line_, col_);
            }
            break;
        case '+':
            result = Token(TokenType::ADD, "+", line_, col_);
            break;
        case '*':
            result = Token(TokenType::MUL, "*", line_, col_);
            break;
        case '/':
            result = Token(TokenType::DIV, "/", line_, col_);
            break;
        case '%':
            result = Token(TokenType::MOD, "%", line_, col_);
            break;
        default:
            if (std::isdigit(last_char)) {
                int value {};

                while (curr_ < content_.length() && std::isdigit(content_[curr_])) {
                    value = value * 10 + (content_[curr_] - '0');
                    ++curr_;
                }

                std::string value_s = std::to_string(value);

                result = Token(TokenType::CONSTANT, value_s, line_, col_);
                col_ += value_s.length() - 1;
                --curr_;
            } else if (std::isalpha(last_char) || last_char == '_') {
                start = curr_;
                while (curr_ < content_.length() && (std::isalnum(content_[curr_]) || content_[curr_] == '_')) {
                    ++curr_;
                }

                std::string name = std::string(content_.substr(start, curr_ - start));
                TokenType result_type = keywords.count(name) ? keywords.at(name) : TokenType::IDENTIFIER;
                result = Token(result_type, name, line_, col_);
                col_ += name.length() - 1;
                --curr_;
            }
    }
    ++col_;
    ++curr_;
    return result;
}