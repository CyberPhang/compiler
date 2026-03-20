#ifndef ASMTREE_H
#define ASMTREE_H

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include "tac.hpp"

namespace ASMTree {
    struct Imm {
        int val;

        Imm(int val);
    };

    struct Reg {
        enum class reg {
            AX,
            DX,
            R10,
            R11,
        };

        Reg::reg r;

        Reg(Reg::reg r);
    };

    struct Pseudo {
        std::string identifier;

        Pseudo(std::string identifier);
    };

    struct Stack {
        int offset;

        Stack(int offset);
    };

    using Operand = std::variant<std::monostate, Imm, Reg, Pseudo, Stack>;

    struct Ret {};

    struct AllocateStack {
        int amount;

        AllocateStack(int amount);
    };

    struct Unary {
        enum class UnOp {
            NEG,
            NOT,
        };

        Unary::UnOp op;
        Operand operand;

        Unary(Unary::UnOp op, Operand operand);
    };

    struct Binary {
        enum class BinOp {
            ADD, SUB, MUL
        };

        BinOp binop;
        Operand op1;
        Operand op2;

        Binary(BinOp binop, Operand op1, Operand op2);
    };

    struct Idiv {
        Operand op;

        Idiv(Operand op);
    };

    struct Cdq {};

    struct Mov {
        Operand src;
        Operand dst;

        Mov(Operand src, Operand dst);
    };

    using Instr = std::variant<std::monostate, Ret, Mov, Unary, Binary, Idiv, Cdq, AllocateStack>;

    struct Function {
        std::string identifier;
        std::vector<Instr> instructions;

        Function(std::string identifier);
    };

    struct Program {
        Function f;

        Program(Function f);
    };

    Program lower(const TAC::Program& p);
}
#endif