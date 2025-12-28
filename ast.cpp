#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "ast.hpp"

void Error::syntax_error(int line, int col, std::string_view msg) {
    std::cout << "Syntax error at line " << line << ", column " << col << ": " << msg << "\n";
}

AST::Return::Return(std::unique_ptr<AST::Expr> exp) : exp(std::move(exp)) {}

void AST::Return::accept(AST::StmtVisitor& v) {
    v.visit(*this);
}

AST::Constant::Constant(int val) : val(val) {}

void AST::Constant::accept(AST::ExprVisitor& v) {
    v.visit(*this);
}

AST::Function::Function(std::string name, std::unique_ptr<AST::Stmt> body) : 
    name(std::move(name)), body(std::move(body)) {}

AST::Program::Program(std::unique_ptr<AST::Function> func_def) : func_def(std::move(func_def)) {}

AST::Parser::Parser(const std::vector<Token>& tokens) : 
    curr(0), tokens(tokens) {}

std::unique_ptr<AST::Constant> AST::Parser::parse_int() {
    if (!expect(TokenType::TOKEN_CONSTANT, "expected constant")) {
        return nullptr;
    }

    auto node = std::make_unique<Constant>(std::move(std::stoi(tokens[curr].lexeme)));
    if (!node) {
        return nullptr;
    }
    ++curr;

    return node;
}

std::unique_ptr<AST::Expr> AST::Parser::parse_exp() {
    return parse_int();
}

std::unique_ptr<AST::Stmt> AST::Parser::parse_statement() {
    if (!expect(TokenType::TOKEN_RET, "expected return")) {
        return nullptr;
    }
    ++curr;
    
    std::unique_ptr<AST::Expr> exp = parse_exp();
    if (!exp) {
        return nullptr;
    }
    
    if (!expect(TokenType::TOKEN_SEMI, "expected semicolon")) {
        return nullptr;
    }
    ++curr;

    return std::make_unique<AST::Return>(std::move(exp));
}

std::unique_ptr<AST::Function> AST::Parser::parse_function() {
    if (!expect(TokenType::TOKEN_INT, "expected 'int' return type")) {
        return nullptr;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_IDENTIFIER, "expected identifier")) {
        return nullptr;
    }
    std::string name = tokens[curr].lexeme;
    ++curr;

    if (!expect(TokenType::TOKEN_OPEN_PARAN, "expected '('")) {
        return nullptr;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_VOID, "expected 'void'")) {
        return nullptr;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_CLOSED_PARAN, "expected ')'")) {
        return nullptr;
    }
    ++curr;

    if (!expect(TokenType::TOKEN_OPEN_BRACE, "expected '{'")) {
        return nullptr;
    }
    ++curr;

    std::unique_ptr<AST::Stmt> body = parse_statement();
    if (!body) {
        return nullptr;
    }
    
    if (!expect(TokenType::TOKEN_CLOSED_BRACE, "expected '}'")) {
        return nullptr;
    }
    ++curr;

    return std::make_unique<AST::Function>(std::move(name), std::move(body));
}

std::unique_ptr<AST::Program> AST::Parser::parse_program() {
    std::unique_ptr<AST::Function> func_def = parse_function();
    if (!func_def) {
        return nullptr;
    }

    if (curr < tokens.size() - 1) {
        Error::syntax_error(tokens[curr].line, tokens[curr].col, "expected program end");
        return nullptr;
    }

    return std::make_unique<AST::Program>(std::move(func_def));
}

bool AST::Parser::expect(TokenType expected, std::string_view msg) {
    const Token &actual = tokens[curr];
    if (actual.type != expected) {
        Error::syntax_error(actual.line, actual.col, msg);
        return false;
    }
    return true;
}