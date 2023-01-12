#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "../lexer/lexer.h"
#include "ast.h"
#include "parser_rules.h"

int parse(struct Token *token, struct Node **ast);
int parseList(struct Token **token, struct Node **ast);
int parseAndOr(struct Token **token, struct Node **ast);
int parsePipeline(struct Token **token, struct Node **ast);
int parseCommand(struct Token **token, struct Node **ast);
int parseSimpleCommand(struct Token **token, struct Node **ast);
struct Node *parseWord(struct Token **token);

#endif /* !PARSER_H */