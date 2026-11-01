#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include "../include/ast.hpp"

void error::syntax_error(int line, int col, std::string_view msg) {
    std::cout << "Syntax error at line " << line << ", column " << col << ": " << msg << "\n";
}

ast::Return::Return(ast::Expr exp) : exp(std::move(exp)) {}

ast::Constant::Constant(int val) : val(val) {}

ast::Unary::Unary(ast::Unary::UnOp op, std::unique_ptr<ast::Expr> exp) : op(op), exp(std::move(exp)) {}

ast::Function::Function(std::string name, ast::Stmt body) : 
    name(std::move(name)), body(std::move(body)) {}

ast::Program::Program(ast::Function func_def) : func_def(std::move(func_def)) {}

ast::Parser::Parser(const std::vector<Token>& tokens) : 
    curr(0), tokens(tokens) {}


std::optional<ast::Constant> ast::Parser::parse_int() {
    if (!expect(TokenType::CONSTANT, "expected constant")) {
        return std::nullopt;
    }

    auto node = Constant(std::move(std::stoi(tokens[curr].lexeme_)));
    ++curr;

    return node;
}

std::optional<ast::Expr> ast::Parser::parse_factor() {
    const Token& token = tokens[curr];
    switch (token.type_) {
        case TokenType::CONSTANT:
            return parse_int();
        case TokenType::NEG:
        case TokenType::TILDE: {
            ast::Unary::UnOp op = token.type_ == TokenType::NEG ? ast::Unary::UnOp::NEG : ast::Unary::UnOp::TILDE;
            ++curr;

            std::optional<ast::Expr> inner_exp = parse_factor();
            if (inner_exp == std::nullopt) {
                return std::nullopt;
            }

            std::unique_ptr<ast::Expr> ptr = std::make_unique<Expr>(std::move(*inner_exp));

            return ast::Unary(op, std::move(ptr));
        }
        case TokenType::OPEN_PARAN: {
            ++curr;

            std::optional<ast::Expr> inner_exp = parse_exp();

            if (!inner_exp || !expect(TokenType::CLOSED_PARAN, "Expected ')'")) {
                return std::nullopt;
            }
            ++curr;
            
            return inner_exp;
        }
        default:
            error::syntax_error(tokens[curr].line_, tokens[curr].col_, "Malformed factor");
            return std::nullopt;
    }
}

bool is_bin_op(const TokenType& t) {
    return t == TokenType::ADD || t == TokenType::NEG || t == TokenType::MUL || 
            t == TokenType::DIV || t == TokenType::MOD;
}

ast::Binary::BinOp get_bin_op(const TokenType& t) {
    switch (t) {
        case TokenType::ADD:
            return ast::Binary::BinOp::ADD;
        case TokenType::NEG:
            return ast::Binary::BinOp::SUB;
        case TokenType::MUL:
            return ast::Binary::BinOp::MUL;
        case TokenType::DIV:
            return ast::Binary::BinOp::DIV;
        case TokenType::MOD:
            return ast::Binary::BinOp::MOD;
        default: // Should never execute
            throw std::runtime_error("Failed to get binary operator");
    }
}

int get_prec(const ast::Binary::BinOp& op) {
    switch (op) {
        case ast::Binary::BinOp::ADD:
        case ast::Binary::BinOp::SUB:
            return 45;
        case ast::Binary::BinOp::MUL:
        case ast::Binary::BinOp::DIV:
        case ast::Binary::BinOp::MOD:
            return 50;
        default: // Should never execute
            throw std::runtime_error("Failed to get precedent of binary operation");
    }
}

std::optional<ast::Expr> ast::Parser::parse_exp(int min_prec = 0) {
    std::optional<ast::Expr> left = parse_factor();
    
    if (!left) {
        return std::nullopt;
    }

    const Token& token = tokens[curr];
    while (is_bin_op(token.type_) && (static_cast<int>(get_bin_op(token.type_)) >= min_prec)) {
        ast::Binary::BinOp op = get_bin_op(token.type_);
        ++curr;
        std::optional<ast::Expr> right = parse_exp(get_prec(op) + 1);

        if (!right) {
            return std::nullopt;
        }

        left = ast::Binary(op, std::make_unique<Expr>(*left), std::make_unique<Expr>(*right));
    }

    ++curr;

    return left;
}

std::optional<ast::Stmt> ast::Parser::parse_statement() {
    if (!expect(TokenType::RET, "expected return")) {
        return std::nullopt;
    }
    ++curr;
    
    std::optional<ast::Expr> exp = parse_exp();
    
    if (!exp || !expect(TokenType::SEMI, "expected semicolon")) {
        return std::nullopt;
    }
    ++curr;

    return ast::Return(std::move(*exp));
}

std::optional<ast::Function> ast::Parser::parse_function() {
    if (!expect(TokenType::INT, "expected 'int' return type")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::IDENTIFIER, "expected identifier")) {
        return std::nullopt;
    }
    std::string name = tokens[curr].lexeme_;
    ++curr;

    if (!expect(TokenType::OPEN_PARAN, "expected '('")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::VOID, "expected 'void'")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::CLOSED_PARAN, "expected ')'")) {
        return std::nullopt;
    }
    ++curr;

    if (!expect(TokenType::OPEN_BRACE, "expected '{'")) {
        return std::nullopt;
    }
    ++curr;

    std::optional<ast::Stmt> body = parse_statement();
    
    if (!body || !expect(TokenType::CLOSED_BRACE, "expected '}'")) {
        return std::nullopt;
    }
    ++curr;

    return ast::Function(std::move(name), std::move(*body));
}

std::optional<ast::Program> ast::Parser::parse_program() {
    std::optional<ast::Function> func_def = parse_function();
    if (!func_def) {
        return std::nullopt;
    }

    if (curr < tokens.size() - 1) {
        error::syntax_error(tokens[curr].line_, tokens[curr].col_, "expected program end");
        return std::nullopt;
    }

    return ast::Program(std::move(*func_def));
}

bool ast::Parser::expect(TokenType expected, std::string_view msg) {
    const Token &actual = tokens[curr];
    if (actual.type_ != expected) {
        error::syntax_error(actual.line_, actual.col_, msg);
        return false;
    }
    return true;
}