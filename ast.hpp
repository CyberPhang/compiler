#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>
#include "lexer.hpp"

class Error {
public:
    static void syntax_error(int line, int col, std::string_view msg);
};

namespace AST {
    // Expr forward declarations
    // Needed for declaring visitor
    class Constant;

    // Stmt forward declarations
    // Needed for declaring visitor
    class Return;

    struct ExprVisitor {
        virtual ~ExprVisitor() = default;
        virtual void visit(Constant& node) = 0;
    };

    struct StmtVisitor {
        virtual ~StmtVisitor() = default;
        virtual void visit(Return& noded) = 0;
    };

    struct Expr {
        virtual ~Expr() = default;
        virtual void accept(ExprVisitor& v) = 0;
    };

    struct Stmt {
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor& v) = 0;
    };

    class Return : public Stmt {
    public:
        std::unique_ptr<Expr> exp;

        Return(std::unique_ptr<Expr> exp);

        void accept(StmtVisitor& v);
    };

    class Constant : public Expr {
    public:
        // TODO: Expand to more types
        int val;

        Constant(int val);

        void accept(ExprVisitor& v);
    };

    class Function {
    public:
        std::string name;
        std::unique_ptr<Stmt> body;

        Function(std::string name, std::unique_ptr<Stmt> body);
    };

    class Program {
    public:
        std::unique_ptr<Function> func_def;

        Program(std::unique_ptr<Function> func_def); 
    };

    class Parser {
        int curr;
        const std::vector<Token>& tokens;

    public:
        Parser(const std::vector<Token>& tokens);

        std::unique_ptr<Constant> parse_int();

        std::unique_ptr<Expr> parse_exp();

        std::unique_ptr<Stmt> parse_statement();

        std::unique_ptr<Function> parse_function();

        std::unique_ptr<Program> parse_program();

        bool expect(TokenType expected, std::string_view msg);
    };
}

#endif