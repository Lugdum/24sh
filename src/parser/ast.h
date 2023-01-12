#ifndef AST_H
#define AST_H

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
    AST_FOR
};

struct Node
{
    enum ast_type type;
    struct Node **children;
    int nb_children;
    char *value;
};

void free_ast(struct Node *node);

#endif /* !AST_H */