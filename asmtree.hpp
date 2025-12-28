#ifndef ASMTREE_H
#define ASMTREE_H

#include <variant>
#include <string>
#include <memory>
#include <vector>
#include "ast.hpp"

namespace ASMTree {
    enum class Reg {
        EAX,
    };

    struct Imm { 
        int val;
        
        Imm(int val);
    };

    enum class Opcode {
        MOV,
        RET,
    };

    struct Operand {
        std::variant<Reg, Imm> val;

        Operand(Reg r);
        Operand(Imm i);
    };

    struct Mov {
        Opcode op;
        Operand src;
        Operand dst;

        Mov(Operand src, Operand dst);
    };

    struct Ret {
        Opcode op;

        Ret();
    };

    using Instr = std::variant<std::monostate, Mov, Ret>;

    struct Function {
        std::string name;
        std::vector<Instr> instructions;

        Function(std::string name);
    };

    struct Program {
        std::vector<Function> functions;

        Program();
        Program(Function func_def);
    };

    class LoweringVisitor : public AST::ExprVisitor, public AST::StmtVisitor {
        Operand last_result = Operand(Imm(0));
        Function* curr_func = nullptr;
        Program program;

        void visit(AST::Constant& node) override;

        void visit(AST::Return& node) override;

        void visit(AST::Function& node);

    public:
        LoweringVisitor();

        Program lower(AST::Program& node);
    };
}

#endif