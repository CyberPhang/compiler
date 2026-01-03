#include <fstream>
#include <variant>
#include "codegen.hpp"
#include "asmtree.hpp"

template<class... Ts> struct overloaded : Ts... { 
    using Ts::operator()...; 
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string format(const ASMTree::Operand& op) {
    return std::visit(overloaded {
        [](const ASMTree::Reg& r) -> std::string {
            switch(r.r) {
                case ASMTree::Reg::reg::AX: return "%eax";
                case ASMTree::Reg::reg::R10: return "%r10d";
                default: return "unknown";
            }
        },
        [](const ASMTree::Imm& i) -> std::string {
            return "$" + std::to_string(i.val);
        },
        [](const ASMTree::Stack& s) -> std::string {
            return std::to_string(s.offset) + "(%rbp)";
        },
        [](const ASMTree::Pseudo& p) -> std::string {
            return p.identifier;
        },
        [](const auto&) -> std::string { return "undefined"; }
    }, op);
}

void Emitter::emit(const ASMTree::Program& node, std::ostream& out) {
    out << "    .globl " << node.f.identifier << "\n";
    out << node.f.identifier << ":\n";
    out << "    pushq    %rbp\n";
    out << "    movq    %rsp, %rbp\n";

    for (const auto& instr : node.f.instructions) {
        out << "    ";
        std::visit(overloaded {
            [&out](const ASMTree::Mov& m) -> void {
                out << "movl    " << format(m.src) << ", " << format(m.dst) << "\n";
            },
            [&out](const ASMTree::Ret& r) -> void {
                out << "movq    %rbp, %rsp\n";
                out << "    ";
                out << "popq    %rbp\n";
                out << "    ";
                out << "ret\n";
            },
            [&out](const ASMTree::Unary& u) -> void {
                switch (u.op) {
                    case ASMTree::Unary::UnOp::NEG:
                        out << "negl    ";
                        break;
                    case ASMTree::Unary::UnOp::NOT:
                        out << "notl    ";
                        break;
                }
                out << format(u.operand) << "\n";
            },
            [&out](const ASMTree::AllocateStack& as) -> void {
                out << "subq    $" << as.amount << ", %rsp\n";
            },
            [](const auto&) -> void { }
        }, instr);
    }
}