#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

enum NodeType {
    AND_OR,
    LIST,
    PIPELINE,
    COMMAND,
    SIMPLE_COMMAND
};

struct Node {
    enum NodeType type;
    struct Node **children;
    char *value;
};

struct Node *parse(struct Token *token);
struct Node *parseList();
struct Node *parseAndOr();
struct Node *parsePipeline();
struct Node *parseCommand();
struct Node *parseSimpleCommand();
void prettyprint(struct Node *node, int level);

#endif /* !PARSER_H */