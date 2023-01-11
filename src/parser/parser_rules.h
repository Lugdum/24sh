#ifndef PARSER_RULES_H
#define PARSER_RULES_H

#include <stdio.h>

#include "../lexer/lexer.h"
#include "parser.h"

struct Node *parseIf(struct Token **token);
struct Node *parseFor(struct Token **token);

#endif /* !PARSER_RULES_H */