#include "parser_rules.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Parser les IFs
int parseIf(struct Token **token, struct Node **if_node)
{
    // Creer le noeud if
    *if_node = calloc(1, sizeof(struct Node));
    if (if_node == NULL)
        return 1;
    (*if_node)->type = AST_IF;

    (*token) = (*token)->next;

    // Faire la condition
    (*if_node)->children = calloc(1, sizeof(struct Node *));
    if ((*if_node)->children == NULL)
        goto error;
    parseAndOr(token, &(*if_node)->children[0]);
    if ((*if_node)->children[0] == NULL)
        goto error;
    (*if_node)->nb_children = 1;

    // Verifier qu'il y a bien then
    if ((*token) == NULL || (*token)->type != THEN)
        goto error;

    (*token) = (*token)->next;

    // Faire le then
    (*if_node)->children = realloc((*if_node)->children, 2 * sizeof(struct Node *));
    if ((*if_node)->children == NULL)
        goto error;
    parseAndOr(token, &(*if_node)->children[1]);
    if ((*if_node)->children[1] == NULL)
        goto error;
    (*if_node)->nb_children = 2;

    // Faire le else s'il y en a un
    if ((*token) != NULL && (*token)->type == ELSE)
    {
        (*token) = (*token)->next;

        (*if_node)->children =
            realloc((*if_node)->children, 3 * sizeof(struct Node *));
        if ((*if_node)->children == NULL)
            goto error;
        parseAndOr(token, &(*if_node)->children[2]);
        if ((*if_node)->children[2] == NULL)
            goto error;
        (*if_node)->nb_children = 3;
    }

    // Verifier que le if termine par fi
    if ((*token) == NULL || (*token)->type != FI)
        goto error;

    (*token) = (*token)->next;
    return 0;

error:
    // freeNode(if_node);
    return 2;
}

// Parser les for
int parseFor(struct Token **token, struct Node **ast)
{
    (*token) = (*token)->next;

    // Variable du for
    if (*token == NULL || (*token)->type != WORD)
        return 2;
    char *var = (*token)->value;
    (*token) = (*token)->next;

    if (*token == NULL || (*token)->type != IN)
        return 2;
    (*token) = (*token)->next;

    // La condition
    struct Node *cond = NULL;
    parseAndOr(token, &cond);

    if (*token == NULL || (*token)->type != DO)
        return 2;
    (*token) = (*token)->next;

    // La boucle
    struct Node *loop = NULL;
    parseAndOr(token, &loop);

    if (*token == NULL || (*token)->type != DONE)
        return 2;
    (*token) = (*token)->next;

    // Faire le if
    *ast = calloc(1, sizeof(struct Node));
    if (ast == NULL)
        return 0;
    (*ast)->type = AST_FOR;
    (*ast)->value = var;
    (*ast)->children = calloc(2, sizeof(struct Node *));
    if ((*ast)->children == NULL)
    {
        free(ast);
        return 2;
    }
    (*ast)->children[0] = cond;
    (*ast)->children[1] = loop;
    (*ast)->nb_children = 2;

    return 0;
}
