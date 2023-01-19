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
    else if(token->type == B_OP)
        res = parseBlockCommand(&token, ast);
    
    // Erreur de syntax
    return 2;
}


int parseBlockCommand(struct Token **token, struct Node **ast)
{
    // Skip '{'
    (*token) = (*token)->next;
    struct Node *block = calloc(1, sizeof(struct Node));
    if (block == NULL)
        return 1;
    block->type = AST_BLOCK;
    block->children = calloc(1, sizeof(struct Node *));
    if (block->children == NULL)
        goto error;

    // parse commands in block
    int res = parseList(token, ast);
    block->children[0] = *ast;
    block->nb_children = 1;
    if(res)
        return res;

    // check for '}'
    if((*token)->type != B_CL)
        return 2;
    // Skip '}'
    (*token) = (*token)->next;

    *ast = block;
    return res;

error:
    free(block->children);
    free(block);
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

    // Creer le noeud liste
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
    while (*token != NULL && (*token)->type == SC)
    {
        if ((*token)->type == SC)
        {
            // Skip ';'
            (*token) = (*token)->next;
            if ((*token)->type == EF || (*token)->type == DONE || (*token)->type == ELSE || (*token)->type == FI)
                break;
            res = parseAndOr(token, ast);
        }
        else if((*token)->type == B_OP)
            res = parseBlockCommand(token, ast);
        else
            return 2;
        list->children = realloc(list->children, (i + 1) * sizeof(struct Node *));
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


int parseRule(struct Token **token, struct Node **ast)
{
    int res = 0;
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


// Parser les ! (ne sert a rien)
int parseEM(struct Token **token, struct Node **ast)
{
    if ((*token)->type != EM)
        return 0;
    struct Node *em = calloc(1, sizeof(struct Node));
    if (em == NULL)
        return 1;
    em->type = AST_EM;
    (*token) = (*token)->next;
    *ast = em;
    return 0;
}


// Parser les conditions AND OR
int parseAndOr(struct Token **token, struct Node **ast)
{
    // Parser les rules ou les ! s'il y en a
    if (parseRule(token, ast))
        return 2;

    // Parser le premier élément de AND_OR
    if (((*token)->type == WORD || (*token)->type == EM) && parsePipeline(token, ast))
        return 2;
    
    int res = 0;
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

    return res;
}


// Parser les pipelines
int parsePipeline(struct Token **token, struct Node **ast)
{
    // Parser le premier element de PIPELINE
    struct Node *pipeline = calloc(1, sizeof(struct Node));
    pipeline->children = calloc(1, sizeof(struct Node *));
    pipeline->nb_children = 1;
    // S'il y a !
    if ((*token)->type == EM)
    {
        pipeline->type = AST_EM;
        (*token) = (*token)->next;
    }
    else
        pipeline->type = AST_PIPELINE;
    int res = parseCommand(token, &pipeline->children[0]);
    *ast = pipeline;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Skip '|'
        (*token) = (*token)->next;
        if ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;
        
        // S'il y a !
        enum TokenType type = AST_PIPELINE;
        if ((*token)->type == EM)
        {
            (*token) = (*token)->next;
            type = AST_EM;
        }
        struct Node *right = NULL;
        parseCommand(token, &right);

        // Creer le noeud PIPELINE et donner les commandes a droite et a gauche
        struct Node *pipeline = calloc(1, sizeof(struct Node));
        if (pipeline == NULL)
            return 1;
        pipeline->type = type;
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
    if ((*ast)->type != AST_EM)
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