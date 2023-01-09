#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Fonction principale
struct Node *parse(struct Token *token)
{
    struct Node *ast = NULL;
    // Cas d'erreur
    if (token == NULL)
        return ast;
    if (token->type == EF || token->type == NL)
        return ast;

    // Parser
    ast = parseList(&token, 0);

    // On est bon
    if (token->type == EF || token->type == NL)
        return ast;

    // Erreur de syntax
    printf("Erreur de syntaxe : la liste ne se termine pas par EOF ou '\\n'\n");
    return NULL;
}

// Parser les listes
struct Node *parseList(struct Token **token, int compound)
{
    struct Node *ast = NULL;
    // Parser le premier élément de la liste
    ast = parseAndOr(token);

    if (*token == NULL)
        return ast;

    // Creer le noeud liste blabla
    struct Node *list = calloc(1, sizeof(struct Node));
    if (list == NULL)
        return NULL;
    list->type = AST_LIST;
    list->children = calloc(1, sizeof(struct Node*));
    if (list->children == NULL)
        goto error;
    list->children[0] = ast;
    list->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token != NULL && (*token)->type == SC && (*token)->type != EF && ((*token)->type != NL || compound))
    {
        // Skip ';'
        (*token) = (*token)->next;
        ast = parseAndOr(token);
        list->children = realloc(list->children, (i+1) * sizeof(struct Node*));
        list->nb_children += 1;
        if (list->children == NULL)
            goto error;
        list->children[i] = ast;

        i++;
    }

    return list;

error:
    free(list);
    return NULL;
}

// Parser les conditions AND OR
struct Node *parseAndOr(struct Token **token)
{
    struct Node *ast = NULL;
    // Parser le premier élément de AND_OR
    ast = parsePipeline(token);

    if (*token == NULL)
        return ast;

    // S'il y a plusieurs trucs
    while (*token != NULL && ((*token)->type == AND || (*token)->type == OR))
    {
        // Skip 'AND' ou 'OR'
        enum TokenType op = (*token)->type;
        (*token) = (*token)->next;
        struct Node *right = parsePipeline(token);

        // Creer le noeud AND_OR donner les commandes a droite et a gauche
        struct Node *and_or = calloc(1, sizeof(struct Node));
        if (and_or == NULL)
            return NULL;
        and_or->type = op;
        and_or->children = calloc(2, sizeof(struct Node*));
        if (and_or->children == NULL)
        {
            free(and_or);
            return NULL;
        }
        and_or->children[0] = ast;
        and_or->children[1] = right;
        and_or->nb_children = 2;
        ast = and_or;
    }

    // S'il y a un if le parser
    if (*token != NULL && (*token)->type == IF)
    {
        struct Node *if_node = parseIf(token);
        if (if_node != NULL)
            return if_node;
    }

    return ast;
}

// Parser les pipelines
struct Node *parsePipeline(struct Token **token)
{
    struct Node *res = NULL;
    // Parser le premier element de PIPELINE
    res = parseCommand(token);

    if (*token == NULL)
        return res;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Skip '|'
        (*token) = (*token)->next;
        struct Node *right = parseCommand(token);

        // Creer le noeud PIPELINE et donner les commandes a droite et a gauche
        struct Node *pipeline = calloc(1, sizeof(struct Node));
        if (pipeline == NULL)
            return NULL;
        pipeline->type = AST_PIPELINE;
        pipeline->children = calloc(2, sizeof(struct Node*));
        if (pipeline->children == NULL)
        {
            free(pipeline);
            return NULL;
        }
        pipeline->children[0] = res;
        pipeline->children[1] = right;
        pipeline->nb_children = 2;
        res = pipeline;
    }

    return res;
}

// Parser les commandes
struct Node *parseCommand(struct Token **token)
{
    struct Node *ast = calloc(1, sizeof(struct Node));
    if (ast == NULL)
        return NULL;

    // Parser le premier element de la commande
    ast->children = calloc(1, sizeof(struct Node*));
    ast->type = AST_COMMAND;
    if (ast->children == NULL)
        goto error;
    ast->children[0] = parseSimpleCommand(token);
    if (!ast->children[0])
        return NULL;
    ast->nb_children = 1;

    // S'il y a plusieurs trucs
    int i = 1;
    while (*token != NULL && (*token)->type >= SC)
    {
        (*token) = (*token)->next;
        ast->children = realloc(ast->children, (i+1) * sizeof(struct Node*));
        ast->nb_children += 1;
        if (ast->children == NULL)
            goto error;
        ast->children[i] = parseSimpleCommand(token);

        i++;
    }

    return ast;

error:
    free(ast);
    return NULL;
}

// Parser les commandes simples
struct Node *parseSimpleCommand(struct Token **token)
{
    struct Node *res = NULL;
    // Parser le premier élément de la SIMPLE_COMMAND
    res = parseWord(token);

    if (*token == NULL)
        return res;

    // S'il y a plusieurs trucs
    while (*token != NULL && (*token)->type == WORD)
    {
        struct Node *word = parseWord(token);

        // Creer le noeud WORD et les ajouter en enfants
        if (!res->nb_children)
        {
            struct Node *new_word = calloc(1, sizeof(struct Node));
            if (new_word == NULL)
                return NULL;
            new_word->type = AST_SIMPLE_COMMAND;
            new_word->children = calloc(2, sizeof(struct Node*));
            if (new_word->children == NULL)
            {
                free(new_word);
                return NULL;
            }
            new_word->children[0] = res;
            new_word->children[1] = word;
            new_word->nb_children = 2;
            res = new_word;
        }
        // Si le noeud existe deja ajouter a ses enfants en agrandissant la liste
        else
        {
            int i = 0;
            while (res->children[i] != NULL)
                i++;
            res->children = realloc(res->children, (i+2) * sizeof(struct Node*));
            if (res->children == NULL)
                return NULL;
            res->children[i] = word;
            res->nb_children = i + 1;
            res->children[i+1] = NULL;
        }
    }

    return res;
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
    if_node->children = calloc(1, sizeof(struct Node*));
    if (if_node->children == NULL)
        goto error;
    if_node->children[0] = parseList(token, 1);
    if (if_node->children[0] == NULL)
        goto error;
    if_node->nb_children = 1;

    // Verifier qu'il y a bien then
    if ((*token) == NULL || (*token)->type != THEN)
        goto error;

    (*token) = (*token)->next;

    // Faire le then
    if_node->children = realloc(if_node->children, 2 * sizeof(struct Node*));
    if (if_node->children == NULL)
        goto error;
    if_node->children[1] = parseList(token, 1);
    if (if_node->children[1] == NULL)
        goto error;
    if_node->nb_children = 2;

    // Faire le else s'il y en a un
    if ((*token) != NULL && (*token)->type == ELSE)
    {
        (*token) = (*token)->next;

        if_node->children = realloc(if_node->children, 3 * sizeof(struct Node*));
        if (if_node->children == NULL)
            goto error;
        if_node->children[2] = parseList(token, 1);
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
    //freeNode(if_node);
    return NULL;
}


// ----------------------------------------------------------------
// Print AST in dot
void prettyprint(struct Node *ast) {
    printf("digraph ast {\n");
    printf("node [shape=box];\n");
    print_node(ast, 0);
    printf("}\n");
}

void print_node(struct Node *node, int parent) {
    static int node_count = 1;
    int current_node = node_count++;
    switch (node->type) {
        case AST_INPUT:
            printf("node%d [label=\"AST_INPUT\"];\n", current_node);
            break;
        case AST_AND_OR:
            printf("node%d [label=\"AND/OR\"];\n", current_node);
            break;
        case AST_LIST:
            printf("node%d [label=\"LIST\"];\n", current_node);
            break;
        case AST_PIPELINE:
            printf("node%d [label=\"PIPELINE\"];\n", current_node);
            break;
        case AST_COMMAND:
            printf("node%d [label=\"COMMAND\"];\n", current_node);
            break;
        case AST_SIMPLE_COMMAND:
            printf("node%d [label=\"SIMPLE_COMMAND\"];\n", current_node);
            break;
        case AST_ELEMENT:
            printf("node%d [label=\"%s\"];\n", current_node, node->value);
            break;
        case AST_CMD:
            printf("node%d [label=\"CMD\"];\n", current_node);
            break;
        case AST_IF:
            printf("node%d [label=\"IF\"];\n", current_node);
            break;
        default:
            printf("node%d [label=\"UNKNOWN\"];\n", current_node);
            break;
    }
    if (parent != 0) {
        printf("node%d -> node%d;\n", parent, current_node);
    }
    for (int i = 0; i < node->nb_children; i++) {
        print_node(node->children[i], current_node);
    }
}