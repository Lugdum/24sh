#ifndef PARSER_RULES_H
#define PARSER_RULES_H

#include <stdio.h>

#include "../lexer/lexer.h"
#include "ast.h"

int parseIf(struct Token **token, struct Node **if_node);
int parseFor(struct Token **token, struct Node **ast);

#endif /* !PARSER_RULES_H */