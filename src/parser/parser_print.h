#ifndef PARSER_PRINT_H
#define PARSER_PRINT_H

#include "parser.h"
#include "../lexer/lexer.h"

#include <stdio.h>

void prettyprint(struct Node *ast, FILE *f);
void print_node(struct Node *node, int parent, FILE *f);
void sexyprint(struct Node *ast);

#endif /* !PARSER_PRINT_H */