#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function.h"

extern struct Function *functions;

// Fonction principale
int parse(struct Token *token, struct Node **ast)
{
    // Cas d'erreur
    if (token == NULL || token->type == EF || token->type == NL)
        return 0;

    // Parser
    int res = parseList(&token, ast);

    // On est bon
    if (token && (token->type == EF || token->type == NL))
        return res;

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
    if (res)
        return res;

    // check for '}'
    if ((*token)->type != B_CL)
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
        // Skip ';'s
        while ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;

        if ((*token)->type == EF || (*token)->type == DONE
            || (*token)->type == ELSE || (*token)->type == FI
            || (*token)->type == B_CL)
            break;

        struct Node *tmp = NULL;
        if (parseAndOr(token, &tmp))
            goto error;

        list->children =
            realloc(list->children, (i + 1) * sizeof(struct Node *));
        list->nb_children += 1;
        if (list->children == NULL)
            goto error;
        list->children[i] = tmp;

        i++;
    }

    *ast = list;

    /*if ((*token)->type == B_OP && parseBlockCommand(token, ast))
        return 2;*/

    return res;

error:
    free_ast(list);
    return 2;
}

int parseRule(struct Token **token, struct Node **ast)
{
    int res = 0;
    // S'il y a une rule la parser
    if (*token != NULL)
    {
        switch ((*token)->type)
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
    int res = 0;

    // Parser les rules ou les ! s'il y en a
    if (parseRule(token, ast))
        return 2;

    // Parser le premier élément de AND_OR
    if (((*token)->type == WORD || (*token)->type == EM)
        || (*token)->type == B_OP)
        res = parsePipeline(token, ast);

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
        switch (op)
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
struct Node *inutile(struct Token **token)
{
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
    return pipeline;
}

int parsePipeline(struct Token **token, struct Node **ast)
{
    // Parser le premier element de PIPELINE
    struct Node *pipeline = inutile(token);

    int res = 0;
    if ((*token)->type == B_OP)
        res = parseBlockCommand(token, &pipeline->children[0]);
    else
        res = parseCommand(token, &pipeline->children[0]);

    if (*ast == NULL)
        *ast = pipeline;
    else
    {
        *ast = realloc(*ast, pipeline->nb_children * sizeof(struct Node *));
        (*ast)->children[(*ast)->nb_children] = pipeline;
        (*ast)->nb_children += 1;
    }

    if (res)
        goto error;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Skip '|'
        (*token) = (*token)->next;
        if ((*token)->type == SC || (*token)->type == NL)
            (*token) = (*token)->next;

        // S'il y a !
        enum ast_type type = AST_PIPELINE;
        if ((*token)->type == EM)
        {
            (*token) = (*token)->next;
            type = AST_EM;
        }
        struct Node *right = NULL;
        if (parseCommand(token, &right))
            goto error;

        // Creer le noeud PIPELINE et donner les commandes a droite et a gauche
        struct Node *pipeline = calloc(1, sizeof(struct Node));
        pipeline->type = type;
        pipeline->children = calloc(2, sizeof(struct Node *));
        
        pipeline->children[0] = *ast;
        pipeline->children[1] = right;
        pipeline->nb_children = 2;
        *ast = pipeline;
    }

    return res;

error:
    free_ast(*ast);
    return 2;
}

// Parser les commandes
int parseCommand(struct Token **token, struct Node **ast)
{
    struct Node *command = calloc(1, sizeof(struct Node));

    // Parser le premier element de la commande
    (command)->children = calloc(1, sizeof(struct Node *));
    (command)->type = AST_COMMAND;
    if ((command)->children == NULL)
        goto error;
    parseSimpleCommand(token, &(command)->children[0]);
    if (!(command)->children[0])
        goto error;
    (command)->nb_children = 1;

    char **word = &command->children[0]->children[0]->value;
    struct Function *found = findFunction(*word);
    int in_func = 0;
    if (found)
    {
        in_func = 1;
        command->function = found;
        command->type = AST_FUNCTION;
    }
    if (strlen(*word) > 1 && (*word)[strlen(*word) - 2] == '('
        && (*word)[strlen(*word) - 1] == ')')
    {
        in_func = 1;
        char *tw = *(word + strlen(*word) - 3);
        free(tw);
        found = findFunction(*word);
        if ((found && parseFunctionReplace(token, found))
            || (!found && parseFunction(token, *word)))
            goto error;
        command->type = AST_CRET_FUNC;
    }
    if (in_func)
    {
        free_ast(command->children[0]);
        command->nb_children = 0;
    }

    if (*ast == NULL)
        *ast = command;
    else
    {
        *ast = realloc(*ast, command->nb_children * sizeof(struct Node *));
        (*ast)->children[(*ast)->nb_children] = command;
        (*ast)->nb_children += 1;
    }

    return 0;

error:
    free_ast(*ast);
    free_ast(command);
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
    strcpy(word->value, ((*token)->value));
    (*token) = (*token)->next;
    /*if (((*token)->type == SC || (*token)->type == NL))
        (*token) = (*token)->next;*/

    return word;
}
