#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include "../include/ast.hpp"

void Error::syntax_error(int line, int col, std::string_view msg) {
    std::cout << "Syntax error at line " << line << ", column " << col << ": " << msg << "\n";
}

AST::Return::Return(AST::Expr exp) : exp(std::move(exp)) {}

AST::Constant::Constant(int val) : val(val) {}

AST::Unary::Unary(AST::Unary::UnOp op, std::unique_ptr<AST::Expr> exp) : op(op), exp(std::move(exp)) {}

AST::Function::Function(std::string name, AST::Stmt body) : 
    name(std::move(name)), body(std::move(body)) {}

AST::Program::Program(AST::Function func_def) : func_def(std::move(func_def)) {}

AST::Parser::Parser(const std::vector<Token>& tokens) : 
    curr(0), tokens(tokens) {}


std::optional<AST::Constant> AST::Parser::parse_int() {
    if (!expect(TokenType::TOKEN_CONSTANT, "expected constant")) {
        return std::nullopt;
    }

    auto node = Constant(std::move(std::stoi(tokens[curr].lexeme)));
    ++curr;

    return node;
}

std::optional<AST::Expr> AST::Parser::parse_factor() {
    const Token& token = tokens[curr];
    switch (token.type) {
        case TokenType::TOKEN_CONSTANT:
            return parse_int();
        case TokenType::TOKEN_NEG:
        case TokenType::TOKEN_TILDE: {
            AST::Unary::UnOp op = token.type == TokenType::TOKEN_NEG ? AST::Unary::UnOp::NEG : AST::Unary::UnOp::TILDE;
            ++curr;

            std::optional<AST::Expr> inner_exp = parse_factor();
            if (inner_exp == std::nullopt) {
                return std::nullopt;
            }

            std::unique_ptr<AST::Expr> ptr = std::make_unique<Expr>(std::move(*inner_exp));

            return AST::Unary(op, std::move(ptr));
        }
        case TokenType::TOKEN_OPEN_PARAN: {
            ++curr;

            std::optional<AST::Expr> inner_exp = parse_exp();

            if (!inner_exp || !expect(TokenType::TOKEN_CLOSED_PARAN, "Expected ')'")) {
                return std::nullopt;
            }
            ++curr;
            
            return inner_exp;
        }
        default:
            Error::syntax_error(tokens[curr].line, tokens[curr].col, "Malformed factor");
            return std::nullopt;
    }
}

bool is_bin_op(const TokenType& t) {
    return t == TokenType::TOKEN_ADD || t == TokenType::TOKEN_NEG || t == TokenType::TOKEN_MUL || 
            t == TokenType::TOKEN_DIV || t == TokenType::TOKEN_MOD;
}

AST::Binary::BinOp get_bin_op(const TokenType& t) {
    switch (t) {
        case TokenType::TOKEN_ADD:
            return AST::Binary::BinOp::ADD;
        case TokenType::TOKEN_NEG:
            return AST::Binary::BinOp::SUB;
        case TokenType::TOKEN_MUL:
            return AST::Binary::BinOp::MUL;
        case TokenType::TOKEN_DIV:
            return AST::Binary::BinOp::DIV;
        case TokenType::TOKEN_MOD:
            return AST::Binary::BinOp::MOD;
        default: // Should never execute
            throw std::runtime_error("Failed to get binary operator");
    }
}

int get_prec(const AST::Binary::BinOp& op) {
    switch (op) {
        case AST::Binary::BinOp::ADD:
        case AST::Binary::BinOp::SUB:
            return 45;
        case AST::Binary::BinOp::MUL:
        case AST::Binary::BinOp::DIV:
        case AST::Binary::BinOp::MOD:
            return 50;
        default: // Should never execute
            throw std::runtime_error("Failed to get precedent of binary operation");
    }
}

std::optional<AST::Expr> AST::Parser::parse_exp(int min_prec = 0) {
    std::optional<AST::Expr> left = parse_factor();
    
    if (!left) {
        return std::nullopt;
    }

    const Token& token = tokens[curr];
    while (is_bin_op(token.type) && (static_cast<int>(get_bin_op(token.type)) >= min_prec)) {
        AST::Binary::BinOp op = get_bin_op(token.type);
        ++curr;
        std::optional<AST::Expr> right = parse_exp(get_prec(op) + 1);

        if (!right) {
            return std::nullopt;
        }

        left = AST::Binary(op, std::make_unique<Expr>(*left), std::make_unique<Expr>(*right));
    }

    ++curr;

    return left;
}

std::optional<AST::Stmt> AST::Parser::parse_statement() {
    if (!expect(TokenType::TOKEN_RET, "expected return")) {
        return std::nullopt;
    }
    ++curr;
    
    std::optional<AST::Expr> exp = parse_exp();
    
    if (!exp || !expect(TokenType::TOKEN_SEMI, "expected semicolon")) {
        return std::nullopt;
    }
    ++curr;

    return AST::Return(std::move(*exp));
}

std::optional<AST::Function> AST::Parser::parse_function() {
    if (!expect(TokenType::TOKEN_INT, "expected 'int' return type")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_IDENTIFIER, "expected identifier")) {
        return std::nullopt;
    }
    std::string name = tokens[curr].lexeme;
    ++curr;

    if (!expect(TokenType::TOKEN_OPEN_PARAN, "expected '('")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_VOID, "expected 'void'")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_CLOSED_PARAN, "expected ')'")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_OPEN_BRACE, "expected '{'")) {
        return std::nullopt;
    }
    ++curr;

    std::optional<AST::Stmt> body = parse_statement();
    
    if (!body || !expect(TokenType::TOKEN_CLOSED_BRACE, "expected '}'")) {
        return std::nullopt;
    }
    ++curr;

    return AST::Function(std::move(name), std::move(*body));
}

std::optional<AST::Program> AST::Parser::parse_program() {
    std::optional<AST::Function> func_def = parse_function();
    if (!func_def) {
        return std::nullopt;
    }

    if (curr < tokens.size() - 1) {
        Error::syntax_error(tokens[curr].line, tokens[curr].col, "expected program end");
        return std::nullopt;
    }

    return AST::Program(std::move(*func_def));
}

bool AST::Parser::expect(TokenType expected, std::string_view msg) {
    const Token &actual = tokens[curr];
    if (actual.type != expected) {
        Error::syntax_error(actual.line, actual.col, msg);
        return false;
    }
    return true;
}