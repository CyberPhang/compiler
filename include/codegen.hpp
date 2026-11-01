#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include <variant>
#include "asmtree.hpp"

namespace emitter {

void emit(const ASMTree::Program& node, std::ostream& out);

}

#endif