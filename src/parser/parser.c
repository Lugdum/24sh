#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction principale
int parse(struct Token *token, struct Node **ast)
{
    // Cas d'erreur
    if (token == NULL || token->type == EF || token->type == NL)
        return 0;

    // Parser
    int res = parseList(&token, ast);

    // On est bon
    if (token->type == EF || token->type == NL)
        return res;

    // Erreur de syntax
    return 2;
}


// Parser les listes
int parseList(struct Token **token, struct Node **ast)
{
    // Parser le premier élément de la liste
    int res = parseAndOr(token, ast);
    if (res)
        return res;

    if (*token == NULL)
        return 0;

    // Creer le noeud liste blabla
    struct Node *list = calloc(1, sizeof(struct Node));
    if (list == NULL)
        return 1;
    list->type = AST_LIST;
    list->children = calloc(1, sizeof(struct Node *));
    if (list->children == NULL)
        goto error;
    list->children[0] = *ast;
    list->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token != NULL && (*token)->type == SC && !res)
    {
        // Skip ';'
        (*token) = (*token)->next;
        if ((*token)->type == EF || (*token)->type == DONE)
            break;
        parseAndOr(token, ast);
        list->children =
            realloc(list->children, (i + 1) * sizeof(struct Node *));
        list->nb_children += 1;
        if (list->children == NULL)
            goto error;
        list->children[i] = *ast;

        i++;
    }

    *ast = list;
    return res;

error:
    free(list->children);
    free(list);
    return 2;
}

// Parser les conditions AND OR
int parseAndOr(struct Token **token, struct Node **ast)
{
    // Parser le premier élément de AND_OR
    int res;
    if ((*token)->type == WORD)
        res = parsePipeline(token, ast);

    if (*token == NULL)
        return res;

    // S'il y a plusieurs trucs
    while (*token != NULL && ((*token)->type == AND || (*token)->type == OR))
    {
        // Skip 'AND' ou 'OR'
        enum TokenType op = (*token)->type;
        (*token) = (*token)->next;
        if ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;
        struct Node *right = NULL;
        parsePipeline(token, &right);

        // Creer le noeud AND_OR donner les commandes a droite et a gauche
        struct Node *and_or = calloc(1, sizeof(struct Node));
        if (and_or == NULL)
            return 1;
        switch(op)
        {
            case AND:
                and_or->type = AST_AND;
                break;
            case OR:
                and_or->type = AST_OR;
                break;
            default:
                break;
        }
        and_or->children = calloc(2, sizeof(struct Node *));
        if (and_or->children == NULL)
        {
            free(and_or);
            return 1;
        }
        and_or->children[0] = *ast;
        and_or->children[1] = right;
        and_or->nb_children = 2;
        *ast = and_or;
    }

    // S'il y a une rule la parser
    if(*token != NULL)
    {
        switch((*token)->type)
        {
            case IF:
                res = parseIf(token, ast);
                break;
            case FOR:
                res = parseFor(token, ast);
                break;
            case UNTIL:
                res = parseUntil(token, ast);
                break;
            case WHILE:
                res = parseWhile(token, ast);
                break;
            default:
                break;
        }
    }

    return res;
}


// Parser les pipelines
int parsePipeline(struct Token **token, struct Node **ast)
{
    // Parser le premier element de PIPELINE
    int res = parseCommand(token, ast);

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Skip '|'
        (*token) = (*token)->next;
        if ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;
        struct Node *right = NULL;
        parseCommand(token, &right);

        // Creer le noeud PIPELINE et donner les commandes a droite et a gauche
        struct Node *pipeline = calloc(1, sizeof(struct Node));
        if (pipeline == NULL)
            return 1;
        pipeline->type = AST_PIPELINE;
        pipeline->children = calloc(2, sizeof(struct Node *));
        if (pipeline->children == NULL)
        {
            free(pipeline);
            return 1;
        }
        pipeline->children[0] = *ast;
        pipeline->children[1] = right;
        pipeline->nb_children = 2;
        *ast = pipeline;
    }

    return res;
}


// Parser les commandes
int parseCommand(struct Token **token, struct Node **ast)
{
    *ast = calloc(1, sizeof(struct Node));
    if (*ast == NULL)
        return 2;

    // Parser le premier element de la commande
    (*ast)->children = calloc(1, sizeof(struct Node *));
    (*ast)->type = AST_COMMAND;
    if ((*ast)->children == NULL)
        goto error;
    parseSimpleCommand(token, &(*ast)->children[0]);
    if (!(*ast)->children[0])
        goto error;
    (*ast)->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token != NULL && (*token)->type > SC)
    {
        (*token) = (*token)->next;
        if ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;
        (*ast)->children = realloc((*ast)->children, (i + 1) * sizeof(struct Node *));
        if ((*ast)->children == NULL)
            goto error;
        parseSimpleCommand(token, &(*ast)->children[i]);
        (*ast)->nb_children += 1;

        i++;
    }

    return 0;

error:
    free_ast(*ast);
    return 2;
}


// Parser les commandes simples
int parseSimpleCommand(struct Token **token, struct Node **ast)
{
    *ast = calloc(1, sizeof(struct Node));
    if (*ast == NULL)
        return 2;

    // Parser le premier element de la commande
    (*ast)->children = calloc(1, sizeof(struct Node *));
    (*ast)->type = AST_SIMPLE_COMMAND;
    if ((*ast)->children == NULL)
        goto error;
    (*ast)->children[0] = parseWord(token);
    (*ast)->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token && (*token)->type == WORD)
    {
        (*ast)->children =
            realloc((*ast)->children, (i + 1) * sizeof(struct Node *));
        if ((*ast)->children == NULL)
            return 1;
        struct Node *word = parseWord(token);
        (*ast)->children[i] = word;
        (*ast)->nb_children += 1;
        i++;
    }

    return 0;

error:
    free_ast(*ast);
    return 2;
}


// Parser les mots
struct Node *parseWord(struct Token **token)
{
    struct Node *word = calloc(1, sizeof(struct Node));
    if (word == NULL)
        return NULL;
    word->type = AST_ELEMENT;
    word->nb_children = 0;
    word->value = calloc(strlen((*token)->value) + 1, sizeof(char));
    strcpy(word->value,((*token)->value));
    (*token) = (*token)->next;
    /*if (((*token)->type == SC || (*token)->type == NL))
        (*token) = (*token)->next;*/

    return word;
}