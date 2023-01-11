#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "../lexer/lexer.h"
#include "parser_rules.h"

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

struct Node *parse(struct Token *token);
struct Node *parseList(struct Token **token);
struct Node *parseAndOr(struct Token **token);
struct Node *parsePipeline(struct Token **token);
struct Node *parseCommand(struct Token **token);
struct Node *parseSimpleCommand(struct Token **token);
struct Node *parseWord(struct Token **token);
struct Node *parseToken(struct Token **token);
struct Node *parseIf(struct Token **token);

#endif /* !PARSER_H */