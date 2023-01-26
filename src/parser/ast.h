#ifndef AST_H
#define AST_H

#include "function.h"

enum ast_type
{
    AST_INPUT,
    AST_LIST,
    AST_AND,
    AST_OR,
    AST_PIPELINE,
    AST_COMMAND,
    AST_SIMPLE_COMMAND,
    AST_ELEMENT,
    AST_CMD,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_UNTIL,
    AST_EM,
    AST_BLOCK,
    AST_FUNCTION,
    AST_CRET_FUNC
};

struct Node {
    enum ast_type type;
    struct Node **children;
    int nb_children;
    char *value;
    char **arguments;
    int nb_arguments;
    struct Function *function;
};

void free_ast(struct Node *node);

#endif /* !AST_H */