#include <variant>
#include <string>
#include <memory>
#include <vector>
#include "ast.hpp"
#include "asmtree.hpp"

ASMTree::Imm::Imm(int val) : val(val) {}

ASMTree::Operand::Operand(ASMTree::Reg r) : val(r) {}
ASMTree::Operand::Operand(ASMTree::Imm i) : val(i) {}

ASMTree::Mov::Mov(ASMTree::Operand src, ASMTree::Operand dst) : op(ASMTree::Opcode::RET), src(src), dst(dst) {}

ASMTree::Ret::Ret() : op(ASMTree::Opcode::RET) {}

ASMTree::Function::Function(std::string name) : name(name), instructions(0) {}

ASMTree::Program::Program() = default;
ASMTree::Program::Program(ASMTree::Function func_def) { functions.push_back(func_def); }

void ASMTree::LoweringVisitor::visit(AST::Constant& node) {
    last_result = ASMTree::Operand(Imm(node.val));
}

void ASMTree::LoweringVisitor::visit(AST::Return& node) {
    node.exp->accept(*this);
    curr_func->instructions.emplace_back(ASMTree::Mov{last_result, ASMTree::Operand(ASMTree::Reg::EAX)});
    curr_func->instructions.emplace_back(ASMTree::Ret{});
}

void ASMTree::LoweringVisitor::visit(AST::Function& node) {
    program.functions.emplace_back(node.name);
    curr_func = &program.functions.back();
    node.body->accept(*this);
    curr_func = nullptr;
}

ASMTree::LoweringVisitor::LoweringVisitor() = default;

ASMTree::Program ASMTree::LoweringVisitor::lower(AST::Program& node) {
    visit(*node.func_def);
    return std::move(program);
}