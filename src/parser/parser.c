#include "parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Token *token;

struct Node *parse(struct Token *tok)
{
    token = tok;
    struct Node *res = NULL;
    // Traiter le cas où la liste est vide
    if (token == NULL)
        return res;

    // Traiter le cas où la liste se termine par EOF ou '\n'
    if (token->type == EF || token->type == NL)
        return res;

    // Sinon, parser la liste
    res = parseList();

    // Si la liste se termine par '\n' ou EOF, renvoyer le résultat
    if (token->type == EF || token->type == NL)
        return res;

    // Sinon, il y a une erreur de syntaxe
    printf("Erreur de syntaxe : la liste ne se termine pas par EOF ou '\\n'\n");
    return NULL;
}

struct Node *parseList(void)
{
    struct Node *res = NULL;
    // Parser le premier élément de la liste
    res = parseAndOr();

    // Si la liste est vide, renvoyer le résultat
    if (token == NULL)
        return res;

    // Sinon, il y a au moins un élément suivant
    struct Node *list = calloc(1, sizeof(struct Node));
    if (list == NULL)
        return NULL;

    list->type = LIST;
    list->children = calloc(1, sizeof(struct Node*));
    if (list->children == NULL)
        goto error;
    list->children[0] = res;

    int i = 1;
    while (token != NULL && token->type == SC && token->type != EF && token->type != NL)
    {
        // Consommer le ';' et parser le prochain élément
        token = token->next;
        res = parseAndOr();
        list->children = realloc(list->children, (i+1) * sizeof(struct Node*));
        if (list->children == NULL)
            goto error;
        list->children[i] = res;

        i++;
    }

    return list;

error:
    free(list);
    return NULL;
}


struct Node *parseAndOr(void)
{
    struct Node *res = NULL;
    // Parser le premier élément de l'AND_OR
    res = parsePipeline();

    // Si l'AND_OR est vide, renvoyer le résultat
    if (token == NULL)
        return res;

    // Sinon, il y a au moins un élément suivant
    while (token != NULL && (token->type == AND || token->type == OR))
    {
        // Consommer le token AND ou OR et parser le prochain élément
        char *v;
        if (token->type == AND)
        {
            v = malloc(4 * sizeof(char));
            if (v == NULL) {
                return NULL;
            }
            strcpy(v, "and");
        }
        else
        {
            v = malloc(3 * sizeof(char));
            if (v == NULL) {
                return NULL;
            }
            strcpy(v, "or");
        }
        token = token->next;
        struct Node *right = parsePipeline();

        // Créer un noeud AND_OR et affecter à ses enfants les noeuds gauche et droit
        struct Node *and_or = calloc(1, sizeof(struct Node));
        if (and_or == NULL)
            return NULL; // Erreur d'allocation de mémoire
        and_or->type = AND_OR;
        and_or->value = v;
        and_or->children = calloc(2, sizeof(struct Node*));
        if (and_or->children == NULL)
        {
            free(and_or);
            return NULL; // Erreur d'allocation de mémoire
        }
        and_or->children[0] = res;
        and_or->children[1] = right;

        // Mettre à jour le noeud résultat
        res = and_or;
    }

    return res;
}

struct Node *parsePipeline(void)
{
    struct Node *res = NULL;
    // Parser le premier élément de la PIPELINE
    res = parseCommand();

    // Si la PIPELINE est vide, renvoyer le résultat
    if (token == NULL)
        return res;

    // Sinon, il y a au moins un élément suivant
    while (token != NULL && token->type == PIPE)
    {
        // Consommer le token PIPE et parser le prochain élément
        token = token->next;
        struct Node *right = parseCommand();

        // Créer un noeud PIPELINE et affecter à ses enfants les noeuds gauche et droit
        struct Node *pipeline = calloc(1, sizeof(struct Node));
        if (pipeline == NULL)
            return NULL;
        pipeline->type = PIPELINE;
        pipeline->children = calloc(2, sizeof(struct Node*));
        if (pipeline->children == NULL)
        {
            free(pipeline);
            return NULL;
        }
        pipeline->children[0] = res;
        pipeline->children[1] = right;

        // Mettre à jour le noeud résultat
        res = pipeline;
    }

    return res;
}

struct Node *parseCommand(void)
{
    struct Node *res = NULL;
    // Si le premier token est un 'if', 'then', 'elif', 'else' ou 'fi', il s'agit d'une structure conditionnelle
    if (token != NULL && (token->type == IF || token->type == THEN || token->type == ELIF || token->type == ELSE || token->type == FI))
    {
        // Créer un noeud COMMAND
        res = calloc(1, sizeof(struct Node));
        if (res == NULL)
            return NULL; // Erreur d'allocation de mémoire
        res->type = token->type;
        res->children = calloc(5, sizeof(struct Node*));
        if (res->children == NULL)
        {
            free(res);
            return NULL; // Erreur d'allocation de mémoire
        }

        // Affecter à chaque enfant le noeud correspondant à la structure conditionnelle
        res->children[0] = parseSimpleCommand();
        token = token->next;
        res->children[1] = parseList();
        //token = token->next;  // je pense que pas necessaire
        res->children[2] = parseSimpleCommand();
        //token = token->next;
        res->children[3] = parseList();
        //token = token->next;
        res->children[4] = parseSimpleCommand();
    }
    else // Sinon, il s'agit d'une commande simple
        res = parseSimpleCommand(token);

    return res;
}

struct Node *parseSimpleCommand()
{
    struct Node *res = NULL;
    // Si la commande est vide, renvoyer NULL
    if (token == NULL)
        return res;

    // Créer un noeud SIMPLE_COMMAND
    res = calloc(1, sizeof(struct Node));
    if (res == NULL)
        return NULL; // Erreur d'allocation de mémoire
    res->type = SIMPLE_COMMAND;
    printf("We ended up here\n");
    res->children = calloc(1, sizeof(struct Node*));
    if (res->children == NULL)
    {
        free(res);
        return NULL; // Erreur d'allocation de mémoire
    }

    // Parser les éléments de type WORD de la commande simple
    int i = 0;
    while (token != NULL && token->type == WORD)
    {
        // Créer un noeud WORD et affecter à son enfant le token courant
        struct Node *word = calloc(1, sizeof(struct Node));
        if (word == NULL)
        {
            free(res->children);
            free(res);
            return NULL; // Erreur d'allocation de mémoire
        }
        word->type = WORD;
        word->value = calloc(strlen(token->value) + 1, sizeof(char));
        if (word->value == NULL)
        {
            free(word);
            free(res->children);
            free(res);
            return NULL; // Erreur d'allocation de mémoire
        }
        strcpy(word->value, token->value);

        // Affecter le noeud WORD à l'enfant de SIMPLE_COMMAND
        res->children = realloc(res->children, (i+1) * sizeof(struct Node*));
        if (res->children == NULL)
        {
            free(word->value);
            free(word);
            free(res);
            return NULL; // Erreur d'allocation de mémoire
        }
        res->children[i] = word;

        // Consommer le token WORD
        token = token->next;
        i++;
    }

    return res;
}

void sexyprint(struct Node *root, int level) 
{
    for (int i = 0; i < level; i++) {
        printf("   |");
    }
    if (level == 0) 
    {
        if (root->value != NULL)
            printf("%s\n", root->value);
        else
            printf("%i\n", root->type);
    }
    else
    {
        if (root->value != NULL)
            printf("-%s\n", root->value);
        else
            printf("-%i\n", root->type);
    }
    int sons = 0;
    if(root->children)
    {
        while (root->children[sons] != NULL)
            sons++;
    }
    for (int i = 0; i < sons; i++)
    {
        sexyprint(root->children[i], level + 1);
    }
}