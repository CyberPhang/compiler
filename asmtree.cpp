#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include "tac.hpp"
#include "asmtree.hpp"

ASMTree::Imm::Imm(int val) : val(val) {}

ASMTree::Reg::Reg(ASMTree::Reg::reg r) : r(r) {}

ASMTree::Pseudo::Pseudo(std::string identifier) : identifier(std::move(identifier)) {}

ASMTree::Stack::Stack(int offset) : offset(offset) {}

ASMTree::AllocateStack::AllocateStack(int amount) : amount(amount) {}

ASMTree::Unary::Unary(ASMTree::Unary::UnOp op, ASMTree::Operand operand) : op(op), operand(std::move(operand)) {}

ASMTree::Mov::Mov(ASMTree::Operand src, ASMTree::Operand dst) : src(std::move(src)), dst(std::move(dst)) {}

ASMTree::Function::Function(std::string identifier) : identifier(std::move(identifier)) {}

ASMTree::Program::Program(ASMTree::Function f) : f(std::move(f)) {}

template <typename T, typename V, typename F>
void if_type(V&& variant, F&& func) {
    if (auto* val = std::get_if<T>(&variant)) {
        func(*val);
    }
}

template<class... Ts> struct overloaded : Ts... { 
    using Ts::operator()...; 
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void replace_operand(int& location, ASMTree::Operand& op, std::unordered_map<std::string, int>& table) {
    if (!std::holds_alternative<ASMTree::Pseudo>(op)) {
        return;
    }

    ASMTree::Pseudo& p = std::get<ASMTree::Pseudo>(op);

    if (!table.count(p.identifier)) {
        location -= sizeof(int);
        table[p.identifier] = location;
        // TODO: When implementing support for more types,
        // encode size information and change accordingly
    }

    op = ASMTree::Stack(table[p.identifier]);
}

int replace_pseudos(std::vector<ASMTree::Instr>& instructions) {
    std::unordered_map<std::string, int> table;
    int loc {};
    for (auto& i : instructions) {
        std::visit( overloaded {
            [&table, &loc](ASMTree::Mov& m) -> void {
                replace_operand(loc, m.src, table);
                replace_operand(loc, m.dst, table);
            },
            [&table, &loc](ASMTree::Unary& u) -> void {
                replace_operand(loc, u.operand, table);
            },
            [](const auto&) -> void {}
        }, i);
    }

    return loc;
}

bool is_invalid_mov(ASMTree::Instr i) {
    auto* instr_ptr = std::get_if<ASMTree::Mov>(&i);
    if (!instr_ptr) {
        return false;
    }

    auto* dst_ptr = std::get_if<ASMTree::Stack>(&(instr_ptr->dst));
    if (!dst_ptr) {
        return false;
    }

    auto* src_ptr = std::get_if<ASMTree::Stack>(&(instr_ptr->src));
    if (!src_ptr) {
        return false;
    }

    return true;
}

auto get_mov_data(const ASMTree::Instr& instr) {
    if (const auto* m = std::get_if<ASMTree::Mov>(&instr)) {
        return std::make_pair(m->src, m->dst);
    }

    throw std::runtime_error("Attempted to cast non-mov instruction to mov\n");
}

void split_invalid_movs(std::vector<ASMTree::Instr>& instructions) {
    int num_invalid_movs { static_cast<int>(std::count_if(
        instructions.begin(), 
        instructions.end(),
        is_invalid_mov) 
    ) };
    
    std::vector<ASMTree::Instr> new_code;
    new_code.reserve(instructions.size() + num_invalid_movs);

    for (auto& instr : instructions) {
        if (is_invalid_mov(instr)) {
            auto [src, dst] = get_mov_data(instr);
            new_code.emplace_back(ASMTree::Mov{src, ASMTree::Reg::reg::R10});
            new_code.emplace_back(ASMTree::Mov(ASMTree::Reg::reg::R10, dst));
        } else {
            new_code.push_back(std::move(instr));
        }
    }

    instructions = std::move(new_code);
}

ASMTree::Operand lower(const TAC::Val& v) {
    return std::visit(
        overloaded {
            [](const TAC::Constant& c) -> ASMTree::Operand { return ASMTree::Imm(c.val); },
            [](const TAC::Var& w) -> ASMTree::Operand { return ASMTree::Pseudo(w.identifier); },
            [](std::monostate) -> ASMTree::Operand { return ASMTree::Imm(0); }
        }, v
    );
}

void lower(const TAC::Instr& i, std::vector<ASMTree::Instr>& instructions) {
    std::visit(
        overloaded {
            [&instructions](TAC::Return r) -> void {
                instructions.emplace_back(ASMTree::Mov{lower(r.val), ASMTree::Reg::reg::AX});
                instructions.emplace_back(ASMTree::Ret{});
            },
            [&instructions](TAC::Unary u) -> void {
                instructions.emplace_back(ASMTree::Mov{lower(u.src), lower(u.dst)});

                ASMTree::Unary::UnOp unop;
                switch (u.op) {
                    case TAC::Unary::UnOp::COMPLEMENT:
                        unop = ASMTree::Unary::UnOp::NOT;
                        break;
                    case TAC::Unary::UnOp::NEGATE:
                        unop = ASMTree::Unary::UnOp::NEG;
                        break;
                }

                instructions.emplace_back(ASMTree::Unary{unop, lower(u.dst)});
            },
            [](std::monostate) -> void {} 
        }, i
    );
}

ASMTree::Function lower(const TAC::Function& f) {
    ASMTree::Function asm_f = ASMTree::Function(f.identifier);
    
    asm_f.instructions.emplace_back(ASMTree::AllocateStack{0});

    for (const auto& i : f.instructions) {
        lower(i, asm_f.instructions);
    }

    int stack_size { - replace_pseudos(asm_f.instructions) };
    split_invalid_movs(asm_f.instructions);
    
    auto& as = std::get<ASMTree::AllocateStack>(asm_f.instructions[0]);
    as.amount = stack_size;
    
    return asm_f;
}

ASMTree::Program ASMTree::lower(const TAC::Program& p) {
    // Due to naming conflict between ASMTree::lower and 
    // lower overloads not in the namespace, explicit 
    // nameless namespace before call to lower is required
    return ASMTree::Program(::lower(p.f));
}