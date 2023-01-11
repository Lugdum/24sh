#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

#include <stdio.h>

enum ast_type
{
    AST_INPUT,
    AST_LIST,
    AST_AND_OR,
    AST_PIPELINE,
    AST_COMMAND,
    AST_SIMPLE_COMMAND,
    AST_ELEMENT,
    AST_CMD,
    AST_IF
};

struct Node {
    enum ast_type type;
    struct Node **children;
    int nb_children;
    char *value;
};

struct Node *parse(struct Token *token);
struct Node *parseList(struct Token **token, int compound);
struct Node *parseAndOr(struct Token **token);
struct Node *parsePipeline(struct Token **token);
struct Node *parseCommand(struct Token **token);
struct Node *parseSimpleCommand(struct Token **token);
struct Node *parseWord(struct Token **token);
struct Node *parseToken(struct Token **token);
struct Node *parseIf(struct Token **token);
void prettyprint(struct Node *ast, FILE *f);
void print_node(struct Node *node, int parent, FILE *f);
void sexyprint(struct Node *ast);

#endif /* !PARSER_H */