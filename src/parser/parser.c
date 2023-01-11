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
    while (*token != NULL && (*token)->type == SC && (*token)->type != EF
           && ((*token)->type != NL))
    {
        // Skip ';'
        (*token) = (*token)->next;
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
    int res = parsePipeline(token, ast);

    if (*token == NULL)
        return res;

    // S'il y a plusieurs trucs
    while (*token != NULL && ((*token)->type == AND || (*token)->type == OR))
    {
        // Skip 'AND' ou 'OR'
        enum TokenType op = (*token)->type;
        (*token) = (*token)->next;
        struct Node *right = NULL;
        parsePipeline(token, &right);

        // Creer le noeud AND_OR donner les commandes a droite et a gauche
        struct Node *and_or = calloc(1, sizeof(struct Node));
        if (and_or == NULL)
            return 1;
        and_or->type = op;
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

    // S'il y a un if le parser
    if (*token != NULL && (*token)->type == IF)
    {
        res = parseIf(token, ast);
    }

    // S'il y a un for le parser
    if (*token != NULL && (*token)->type == FOR)
    {
        res = parseFor(token, ast);
    }

    return res;
}

// Parser les pipelines
int parsePipeline(struct Token **token, struct Node **ast)
{
    // Parser le premier element de PIPELINE
    int res = parseCommand(token, ast);

    if (*token == NULL)
        return 0;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Skip '|'
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
        return 2;
    (*ast)->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token != NULL && (*token)->type >= SC && (*token)->type != EF)
    {
        (*token) = (*token)->next;
        (*ast)->children = realloc((*ast)->children, (i + 1) * sizeof(struct Node *));
        (*ast)->nb_children += 1;
        if ((*ast)->children == NULL)
            goto error;
        parseSimpleCommand(token, &(*ast)->children[i]);

        i++;
    }

    return 0;

error:
    free(ast);
    return 2;
}

// Parser les commandes simples
int parseSimpleCommand(struct Token **token, struct Node **ast)
{
    // Parser le premier élément de la SIMPLE_COMMAND
    *ast = parseWord(token);

    if (*token == NULL)
        return 0;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == WORD)
    {
        struct Node *word = parseWord(token);

        // Creer le noeud WORD et les ajouter en enfants
        if (!(*ast)->nb_children)
        {
            struct Node *new_word = calloc(1, sizeof(struct Node));
            if (new_word == NULL)
                return 1;
            new_word->type = AST_SIMPLE_COMMAND;
            new_word->children = calloc(2, sizeof(struct Node *));
            if (new_word->children == NULL)
            {
                free(new_word);
                return 1;
            }
            new_word->children[0] = *ast;
            new_word->children[1] = word;
            new_word->nb_children = 2;
            *ast = new_word;
        }
        // Si le noeud existe deja ajouter a ses enfants en agrandissant la
        // liste
        else
        {
            int i = 0;
            while ((*ast)->children[i] != NULL)
                i++;
            (*ast)->children =
                realloc((*ast)->children, (i + 2) * sizeof(struct Node *));
            if ((*ast)->children == NULL)
                return 1;
            (*ast)->children[i] = word;
            (*ast)->nb_children = i + 1;
            (*ast)->children[i + 1] = NULL;
        }
    }

    return 0;
}

// Parser les mots
struct Node *parseWord(struct Token **token)
{
    if (!(*token) || (*token)->type != WORD)
        return NULL;

    struct Node *word = calloc(1, sizeof(struct Node));
    if (word == NULL)
        return NULL;
    word->type = AST_ELEMENT;
    word->nb_children = 0;
    word->value = strdup((*token)->value);
    if (word->value == NULL)
    {
        free(word);
        return NULL;
    }
    (*token) = (*token)->next;

    return word;
}