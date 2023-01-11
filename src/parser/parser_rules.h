#ifndef PARSER_RULES_H
#define PARSER_RULES_H

#include "parser.h"
#include "../lexer/lexer.h"

#include <stdio.h>

struct Node *parseIf(struct Token **token);
struct Node *parseFor(struct Token **token);

#endif /* !PARSER_RULES_H */