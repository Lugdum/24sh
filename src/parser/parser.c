#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Node *parse(struct Token *token)
{
    struct Node *ast = NULL;
    // Traiter le cas où la liste est vide
    if (token == NULL)
        return ast;

    // Traiter le cas où la liste se termine par EOF ou '\n'
    if (token->type == EF || token->type == NL)
        return ast;

    // Sinon, parser la liste
    ast = parseList(&token);

    // Si la liste se termine par '\n' ou EOF, renvoyer le résultat
    if (token->type == EF || token->type == NL)
        return ast;

    // Sinon, il y a une erreur de syntaxe
    printf("Erreur de syntaxe : la liste ne se termine pas par EOF ou '\\n'\n");
    return NULL;
}

struct Node *parseList(struct Token **token)
{
    struct Node *ast = NULL;
    // Parser le premier élément de la liste
    ast = parseAndOr(token);

    // Si la liste est vide, renvoyer le résultat
    if (*token == NULL)
        return ast;

    // S'il y a < 1 élément suivant
    struct Node *list = calloc(1, sizeof(struct Node));
    if (list == NULL)
        return NULL;

    list->type = AST_LIST;
    list->children = calloc(1, sizeof(struct Node*));
    if (list->children == NULL)
        goto error;
    list->children[0] = ast;
    list->nb_children = 1;

    int i = 1;
    while (*token != NULL && (*token)->type == SC && (*token)->type != EF && (*token)->type != NL)
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


struct Node *parseAndOr(struct Token **token)
{
    struct Node *ast = NULL;
    // Parser le premier élément de l'AND_OR
    ast = parsePipeline(token);

    // Si l'AND_OR est vide, renvoyer le résultat
    if (*token == NULL)
        return ast;

    // S'il y a < 1 élément suivant
    while (*token != NULL && ((*token)->type == AND || (*token)->type == OR))
    {
        // Skip 'AND' ou 'OR'
        enum TokenType op = (*token)->type;
        (*token) = (*token)->next;
        struct Node *right = parsePipeline(token);

        // Créer un noeud AND_OR et affecter à ses enfants les noeuds gauche et droit
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

        // Mettre à jour le noeud résultat
        ast = and_or;
    }

    return ast;
}

struct Node *parsePipeline(struct Token **token)
{
    struct Node *res = NULL;
    // Parser le premier élément de la PIPELINE
    res = parseCommand(token);

    // Si la PIPELINE est vide, renvoyer le résultat
    if (*token == NULL)
        return res;

    // S'il y a < 1 élément suivant
    while (*token != NULL && (*token)->type == PIPE)
    {
        // Consommer le token PIPE et parser le prochain élément
        (*token) = (*token)->next;
        struct Node *right = parseCommand(token);

        // Créer un noeud PIPELINE et affecter à ses enfants les noeuds gauche et droit
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

        // Mettre à jour le noeud résultat
        res = pipeline;
    }

    return res;
}

struct Node *parseCommand(struct Token **token)
{
    struct Node *ast = NULL;

    // Allouer de la mémoire pour le noeud de commande
    ast = calloc(1, sizeof(struct Node));
    if (ast == NULL)
        return NULL;

    // Parser le premier élément de la commande
    ast->children = calloc(1, sizeof(struct Node*));
    if (ast->children == NULL)
        goto error;
    ast->children[0] = parseSimpleCommand(token);
    ast->nb_children = 1;

    // S'il y a < 1 élément suivant
    int i = 1;
    while (*token != NULL && (*token)->type < SC)
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

struct Node *parseSimpleCommand(struct Token **token)
{
    struct Node *res = NULL;
    // Parser le premier élément de la SIMPLE_COMMAND
    res = parseWord(token);

    // Si la SIMPLE_COMMAND est vide, renvoyer le résultat
    if (*token == NULL)
        return res;

    // S'il y a < 1 élément suivant
    while (*token != NULL && (*token)->type == WORD)
    {
        struct Node *word = parseWord(token);

        // Créer un noeud WORD et ajouter le nouveau WORD en tant qu'enfant
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

struct Node *parseWord(struct Token **token)
{
    if ((*token)->type != WORD)
        return parseToken(token);

    if (*token == NULL)
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

struct Node *parseToken(struct Token **token)
{
    if (*token == NULL)
        return NULL;

    struct Node *tok = calloc(1, sizeof(struct Node));
    if (tok == NULL)
        return NULL;
    tok->type = AST_SIMPLE_COMMAND;
    tok->nb_children = 0;
    switch ((*token)->type)
    {
    case IF:
        tok->value = calloc(2, sizeof(char));
        strcpy(tok->value, "IF");
        break;
    case THEN:
        tok->value = calloc(4, sizeof(char));
        strcpy(tok->value, "THEN");
        break;
    case ELSE:
        tok->value = calloc(4, sizeof(char));
        strcpy(tok->value, "ELSE");
        break;
    case FI:
        tok->value = calloc(2, sizeof(char));
        strcpy(tok->value, "FI");
        break;
    
    default:
        tok->value = calloc(strlen((*token)->value), sizeof(char));
        strcpy(tok->value, (*token)->value);
        break;
    }
    if (tok->value == NULL)
    {
        free(tok);
        return NULL;
    }
    (*token) = (*token)->next;

    return tok;
}

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