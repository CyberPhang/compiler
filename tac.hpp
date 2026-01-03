#ifndef TAC_H
#define TAC_H
#include <string>
#include <variant>
#include <vector>
#include "ast.hpp"

namespace TAC {
    struct Constant {
        int val;

        Constant(int val);
    };

    struct Var {
        std::string identifier;

        Var(std::string identifier);
    };

    using Val = std::variant<std::monostate, Constant, Var>;

    struct Return {
        Val val;

        Return(Val val);
    };

    struct Unary {
        enum class UnOp { COMPLEMENT, NEGATE };
        
        UnOp op;
        Val src;
        Var dst;

        Unary(UnOp op, Val src, Var dst);
    };

    using Instr = std::variant<std::monostate, Return, Unary>;

    struct Function {
        std::string identifier;
        std::vector<Instr> instructions;

        Function(std::string identifier);
    };

    struct Program {
        Function f;

        Program(Function f);
    };

    Program emit_tac(const AST::Program& p);
}
#endif