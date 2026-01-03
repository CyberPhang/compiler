#include <vector>
#include <variant>
#include <string>
#include "tac.hpp"

TAC::Constant::Constant(int val) : val(val) {}

TAC::Var::Var(std::string identifier) : identifier(std::move(identifier)) {}

using Val = std::variant<std::monostate, TAC::Constant, TAC::Var>;

TAC::Return::Return(TAC::Val val) : val(std::move(val)) {}

TAC::Unary::Unary(TAC::Unary::UnOp op, TAC::Val src, TAC::Var dst) : op(op), src(std::move(src)), dst(std::move(dst)) {}

using Instr = std::variant<std::monostate, TAC::Return, TAC::Unary>;

TAC::Function::Function(std::string identifier) : identifier(std::move(identifier)), instructions(0) {}

TAC::Program::Program(Function f) : f(std::move(f)) {}

std::string make_temp() {
    static int counter {};

    return "tmp." + std::to_string(counter++);
}

TAC::Unary::UnOp convert_unop(const AST::Unary::UnOp unop) {
    switch (unop) {
        case AST::Unary::UnOp::NEG:
            return TAC::Unary::UnOp::NEGATE;
        case AST::Unary::UnOp::TILDE:
            return TAC::Unary::UnOp::COMPLEMENT;
        default: // satisfy compiler warning - should never execute
            return TAC::Unary::UnOp::NEGATE;
    }
}

template<class... Ts> struct overloaded : Ts... { 
    using Ts::operator()...; 
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

TAC::Val emit_tac(const AST::Expr& exp, std::vector<TAC::Instr>& instructions) {
    return std::visit(
        overloaded {
            [](const AST::Constant& c) -> TAC::Val { return TAC::Constant(c.val); },
            [&instructions](const AST::Unary& u) -> TAC::Val { 
                TAC::Val src = ::emit_tac(*u.exp, instructions);
                TAC::Var dst = TAC::Var(make_temp());
                TAC::Unary::UnOp op = convert_unop(u.op);
                instructions.emplace_back(std::in_place_type<TAC::Unary>, op, src, dst);
                return dst;
            },
            [](const std::monostate&) -> TAC::Val { return std::monostate{}; }
        }, exp
    );
}

TAC::Function emit_tac(const AST::Function& f) {
    TAC::Function tac_f = TAC::Function(f.name);

    std::visit(
        overloaded {
            [&tac_f](const AST::Return& r) -> void {
                tac_f.instructions.emplace_back(std::in_place_type<TAC::Return>, ::emit_tac(r.exp, tac_f.instructions));
            },
            [](const std::monostate&) -> void {}
        }, f.body
    );

    return tac_f;
}

TAC::Program TAC::emit_tac(const AST::Program& p) {
    TAC::Function tac_func = ::emit_tac(p.func_def);
    return TAC::Program(std::move(tac_func));
}