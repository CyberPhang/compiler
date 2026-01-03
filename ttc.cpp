#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "ast.hpp"
#include "tac.hpp"
#include "asmtree.hpp"
#include "codegen.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./ttc.exe [filename]" << "\n";
        return 1;
    } else {
        std::ifstream input_file(argv[1]);

        if (!input_file.is_open()) {
            std::cerr << "Error: unable to open the file " << argv[1] << "\n";
            return 1;
        }

        std::stringstream buffer;
        buffer << input_file.rdbuf();

        Lexer l = Lexer();
        std::vector<Token> tokens = l.read(buffer.str());
        
        AST::Parser p = AST::Parser(tokens);
        std::optional<AST::Program> ast = p.parse_program();

        if (!ast) {
            std::cerr << "Aborted due to syntax error";
            return 1;
        }

        TAC::Program tac = TAC::emit_tac(*ast);

        ASMTree::Program asm_tree = ASMTree::lower(tac);

        std::ofstream output_file("main.asm");
        
        if (!output_file.is_open()) {
            std::cerr << "Error: unable to open or create file 'main.asm'";
            return 1;
        }

        Emitter::emit(asm_tree, output_file);

        std::cout << "Successfully compiled: main.asm\n";

        output_file.close();
        input_file.close();

        return 0;
    }
}