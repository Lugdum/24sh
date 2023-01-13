#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Token *process_end_of_file(struct Token *tok)
{
    struct Token *token = calloc(1, sizeof(struct Token));
    token->value = NULL;
    token->next = NULL;

    token->type = EF;

    tok->next = token;
    return token;
}

struct Token *process(char *str, struct Token *tok)
{
    struct Token *token = calloc(1, sizeof(struct Token));
    token->value = NULL;
    token->next = NULL;

    if (strlen(str) == 0)
    {
        free(token);
        return tok;
    }
    else if (!strcmp("if", str))
        token->type = IF;
    else if (!strcmp("then", str))
        token->type = THEN;
    else if (!strcmp("elif", str))
        token->type = ELIF;
    else if (!strcmp("else", str))
        token->type = ELSE;
    else if (!strcmp("fi", str))
        token->type = FI;
    else if (!strcmp("|", str))
        token->type = PIPE;
    else if (!strcmp(";", str))
        token->type = SC;
    else if (!strcmp("\n", str))
        token->type = NL;
    else if (!strcmp("'", str))
        token->type = SQ;
    else if (!strcmp("&&", str))
        token->type = AND;
    else if (!strcmp("||", str))
        token->type = OR;
    else if (!strcmp("for", str))
        token->type = FOR;
    else if (!strcmp("in", str))
        token->type = IN;
    else if (!strcmp("do", str))
        token->type = DO;
    else if (!strcmp("done", str))
        token->type = DONE;
    else if (!strcmp("while", str))
        token->type = WHILE;
    else if (!strcmp("until", str))
        token->type = UNTIL;
    else if (!strcmp(" ", str))
    {
        free(token);
        return tok;
    }
    else
    {
        if (str[strlen(str) - 1] == '\\')
            str[strlen(str) - 1] = '\0';
        token->type = WORD;
        token->value = calloc(strlen(str) + 1, sizeof(char));
        if (!token->value)
        {
            free(token);
            return NULL;
        }
        strcpy(token->value, str);
    }
    tok->next = token;
    return token;
}

struct Token *lexer(char *input)
{
   // printf("%s\n", input);
    struct Token *out = calloc(1, sizeof(struct Token));
    if (!out)
        return NULL;
    struct Token *cur_tok = out;

    int len = strlen(input);
    char *cur = calloc(len + 1, 1);
    int j = 0;
    bool quote = false;
    for (int i = 0; i < len; i++)
    {
        if (input[i] == '\"' && input[i-1] != '\\')
        {
            quote = !quote;
            continue;
        }
        if (input[i] == '\\' && !quote)
            continue;
        if (input[i] == ' ' || input[i] == ';' || input[i] == '\n')
        {
            if (!quote)
            {
                cur[j] = '\0';
                cur_tok = process(cur, cur_tok);
    /*            cur[0] = input[i];
                cur[1] = '\0';
                cur_tok = process(cur, cur_tok);*/
                j = 0;
            }
            else
            {
                cur[j] = input[i];
                j++;
            }
        }
        else
        {
            cur[j] = input[i];
            j++;
        }
    }
    cur[j] = '\0';
    cur_tok = process(cur, cur_tok);
    cur_tok = process_end_of_file(cur_tok);

    free(cur);
    cur_tok = out->next;
    free(out);
    return cur_tok;
}

void print_token(struct Token *token)
{
    while (token)
    {
        switch (token->type)
        {
        case IF:
            printf("IF ");
            break;
        case THEN:
            printf("THEN ");
            break;
        case ELIF:
            printf("ELIF ");
            break;
        case ELSE:
            printf("ELSE ");
            break;
        case FI:
            printf("FI ");
            break;
        case AND:
            printf("AND ");
            break;
        case OR:
            printf("OR ");
            break;
        case FOR:
            printf("FOR ");
            break;
        case UNTIL:
            printf("UNTIL ");
            break;
        case WHILE:
            printf("WHILE ");
            break;
        case IN:
            printf("IN ");
            break;
        case DO:
            printf("DO ");
            break;
        case DONE:
            printf("DONE ");
            break;
        case SC:
            printf("; ");
            break;
        case NL:
            printf("\\n ");
            break;
        case SQ:
            printf("' ");
            break;
        case WORD:
            printf("%s ", token->value);
            break;
        case PIPE:
            printf("PIPE ");
            break;
        case EF:
            printf("FIN");
            break;
        default:
            printf("? ");
            break;
        }
        token = token->next;
    }
    printf("\n");
}

void free_lexer(struct Token *token)
{
    while (token != NULL)
    {
        struct Token *next = token->next;
        free(token->value);
        free(token);
        token = next;
    }
}
