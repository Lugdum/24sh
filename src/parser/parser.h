#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"

enum NodeType {
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
struct Node *parseList(struct Token *token);
struct Node *parseAndOr(struct Token *token);
struct Node *parsePipeline(struct Token *token);
struct Node *parseCommand(struct Token *token);
struct Node *parseSimpleCommand(struct Token *token);
void prettyprint(struct Node *node, int level);

#endif /* !PARSER_H */