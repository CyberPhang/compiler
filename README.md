# Teeny Tiny C

A simple compiler for a subset of C, which currently only compiles programs that look like

```c

int main(void) {
    return 0;
}
```

(or with any other integer value). This includes a lexer, an abstract syntax tree, an assembly tree as an intermediate
representation, and an assembly code generator.

The project is built in C++ 17.