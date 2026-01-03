# Teeny Tiny C

A simple compiler for a subset of C, which currently includes support for: 

- The main function (must include void as the parameters)
- Unary operators

The current feature in progress is binary operators.

This compiler includes a lexer, an abstract syntax tree, a three-address-code 
intermediate representation, an AST for the assembly, and finally an assembly code generator.

To use the compiler, first, compile all .cpp files in the source code to generate the 
executable. Then use 

```
$ /ttc [filename]
```

and the compiler will build a file main.asm, in which the corresponding assembly will be printed.

This project is built in C++ 17.