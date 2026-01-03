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

Example:

The following C program

```c
int main(void) {
    return -(~5);
}
```

compiles to

```asm
    .globl main
main:
    pushq    %rbp
    movq    %rsp, %rbp
    subq    $8, %rsp
    movl    $5, -4(%rbp)
    notl    -4(%rbp)
    movl    -4(%rbp), %r10d
    movl    %r10d, -8(%rbp)
    negl    -8(%rbp)
    movl    -8(%rbp), %eax
    movq    %rbp, %rsp
    popq    %rbp
    ret
```


This project is built in C++ 17.