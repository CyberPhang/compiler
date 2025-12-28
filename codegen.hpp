#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include <variant>
#include "asmtree.hpp"

namespace Emitter {
    template<class... Ts> struct overloaded : Ts... { 
        using Ts::operator()...; 
    };

    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    std::string format(const ASMTree::Operand& op);

    void emit(const ASMTree::Program& node, std::ostream& out);
}

#endif