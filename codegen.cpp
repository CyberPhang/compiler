#include <fstream>
#include <variant>
#include "codegen.hpp"
#include "asmtree.hpp"

std::string Emitter::format(const ASMTree::Operand& op) {
    return std::visit(overloaded {
        [](ASMTree::Reg r) -> std::string {
            switch(r) {
                case ASMTree::Reg::EAX: return "%eax";
                default: return "unknown";
            }
        },
        [](ASMTree::Imm i) -> std::string {
            return "$" + std::to_string(i.val);
        }
    }, op.val);
}

void Emitter::emit(const ASMTree::Program& node, std::ostream& out) {
    for (const auto& func : node.functions) {
        out << ".globl" << func.name << "\n";
        out << func.name << ":\n";

        for (const auto& instr : func.instructions) {
            out << "    ";
            std::visit(overloaded {
                [&out](const ASMTree::Mov& m) {
                    out << "mov " << format(m.src) << ", " << format(m.dst) << "\n";
                },
                [&out](const ASMTree::Ret&) {
                    out << "ret\n";
                },
                [&out](const std::monostate&) { out << "ERROR"; }
            }, instr);
        }
    }
}