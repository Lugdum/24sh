#include "parser_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

// Parser les IFs
struct Node *parseIf(struct Token **token)
{
    // Creer le noeud if
    struct Node *if_node = calloc(1, sizeof(struct Node));
    if (if_node == NULL)
        return NULL;
    if_node->type = AST_IF;

    (*token) = (*token)->next;

    // Faire la condition
    if_node->children = calloc(1, sizeof(struct Node *));
    if (if_node->children == NULL)
        goto error;
    if_node->children[0] = parseAndOr(token);
    if (if_node->children[0] == NULL)
        goto error;
    if_node->nb_children = 1;

    // Verifier qu'il y a bien then
    if ((*token) == NULL || (*token)->type != THEN)
        goto error;

    (*token) = (*token)->next;

    // Faire le then
    if_node->children = realloc(if_node->children, 2 * sizeof(struct Node *));
    if (if_node->children == NULL)
        goto error;
    if_node->children[1] = parseAndOr(token);
    if (if_node->children[1] == NULL)
        goto error;
    if_node->nb_children = 2;

    // Faire le else s'il y en a un
    if ((*token) != NULL && (*token)->type == ELSE)
    {
        (*token) = (*token)->next;

        if_node->children =
            realloc(if_node->children, 3 * sizeof(struct Node *));
        if (if_node->children == NULL)
            goto error;
        if_node->children[2] = parseAndOr(token);
        if (if_node->children[2] == NULL)
            goto error;
        if_node->nb_children = 3;
    }

    // Verifier que le if termine par fi
    if ((*token) == NULL || (*token)->type != FI)
        goto error;

    (*token) = (*token)->next;
    return if_node;

error:
    // freeNode(if_node);
    return NULL;
}

// Parser les for
struct Node *parseFor(struct Token **token)
{
    (*token) = (*token)->next;

    // Variable du for
    if (*token == NULL || (*token)->type != WORD)
        return NULL;
    char *var = (*token)->value;
    (*token) = (*token)->next;

    if (*token == NULL || (*token)->type != IN)
        return NULL;
    (*token) = (*token)->next;

    // La condition
    struct Node *cond = parseAndOr(token);

    if (*token == NULL || (*token)->type != DO)
        return NULL;
    (*token) = (*token)->next;

    // La boucle
    struct Node *loop = parseAndOr(token);

    if (*token == NULL || (*token)->type != DONE)
        return NULL;
    (*token) = (*token)->next;

    // Faire le if
    struct Node *ast = calloc(1, sizeof(struct Node));
    if (ast == NULL)
        return NULL;
    ast->type = AST_FOR;
    ast->value = var;
    ast->children = calloc(2, sizeof(struct Node *));
    if (ast->children == NULL)
    {
        free(ast);
        return NULL;
    }
    ast->children[0] = cond;
    ast->children[1] = loop;
    ast->nb_children = 2;

    return ast;
}
