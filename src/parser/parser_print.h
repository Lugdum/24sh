#ifndef PARSER_PRINT_H
#define PARSER_PRINT_H

#include <stdio.h>

#include "../lexer/lexer.h"
#include "parser.h"

void prettyprint(struct Node *ast, FILE *f);
void print_node(struct Node *node, int parent, FILE *f);
void sexyprint(struct Node *ast);

#endif /* !PARSER_PRINT_H */