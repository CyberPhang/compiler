#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include "lexer.hpp"

namespace Error {
    static void syntax_error(int line, int col, std::string_view msg);
};

namespace AST {    
    struct Constant {
        int val;

        Constant(int val);
    };

    using Expr = std::variant<std::monostate, Constant, struct Unary, struct Binary>;

    struct Unary {
        enum class UnOp { NEG, TILDE };

        UnOp op;
        std::unique_ptr<Expr> exp;

        Unary(Unary::UnOp op, std::unique_ptr<Expr> exp);
    };

    struct Binary {
        enum class BinOp { ADD, SUB, MUL, DIV, MOD, };

        BinOp op;
        std::unique_ptr<Expr> lhs;
        std::unique_ptr<Expr> rhs;

        Binary(Binary::BinOp op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);
    };

    using Expr = std::variant<std::monostate, Constant, Unary, Binary>;

    struct Return {
        Expr exp;

        Return(Expr exp);
    };

    using Stmt = std::variant<std::monostate, Return>;

    struct Function {
        std::string name;
        Stmt body;

        Function(std::string name, Stmt body);
    };

    struct Program {
        Function func_def;

        Program(Function func_def); 
    };

    class Parser {
        int curr;
        const std::vector<Token>& tokens;

        std::optional<Constant> parse_int();

        std::optional<Expr> parse_factor();

        std::optional<Expr> parse_exp(int min_prec);

        std::optional<Stmt> parse_statement();

        std::optional<Function> parse_function();

    public:
        Parser(const std::vector<Token>& tokens);

        std::optional<Program> parse_program();

        bool expect(TokenType expected, std::string_view msg);
    };
}

#endif