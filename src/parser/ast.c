#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

void free_ast(struct Node *node)
{
    if (!node)
        return;
    for (int i = 0; i < node->nb_children; i++)
        free_ast(node->children[i]);
    node->nb_children = 0;
    free(node->children);
    free(node->value);
    free(node);
}