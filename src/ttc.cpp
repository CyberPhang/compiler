#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "../include/lexer.hpp"
#include "../include/ast.hpp"
#include "../include/tac.hpp"
#include "../include/asmtree.hpp"
#include "../include/codegen.hpp"

int main(int argc, char* argv[]) {
    // The user should enter a single file to be compiled
    if (argc != 2) {
        std::cout << "Usage: ./ttc.exe [filename]\n";
        return EXIT_FAILURE;
    }
    
    std::ifstream input_file(argv[1]);
    if (!input_file.is_open()) {
        std::cerr << "Error: unable to open the file " << argv[1] << "\n";
        return EXIT_FAILURE;
    }

    // Read in file content
    std::stringstream buffer;
    buffer << input_file.rdbuf();

    // Lex raw characters to generate token stream
    Lexer l = Lexer();
    std::vector<Token> tokens = l.read(buffer.str());
    
    // Parse tokens and generate AST
    AST::Parser p = AST::Parser(tokens);
    std::optional<AST::Program> ast = p.parse_program();
    if (!ast) {
        std::cerr << "Aborted due to syntax error\n";
        return EXIT_FAILURE;
    }

    // Generate TAC IR and emit assembly
    TAC::Program tac = TAC::emit_tac(*ast);
    ASMTree::Program asm_tree = ASMTree::lower(tac);

    std::ofstream output_file("main.asm");
    if (!output_file.is_open()) {
        std::cerr << "Error: unable to open or create file 'main.asm'\n";
        return EXIT_FAILURE;
    }
    Emitter::emit(asm_tree, output_file);

    std::cout << "Successfully compiled: main.asm\n";

    // Cleanup
    output_file.close();
    input_file.close();

    return EXIT_SUCCESS;
}