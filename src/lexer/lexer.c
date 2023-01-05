#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Token *lex(char *arg)
{
    struct Token *res = NULL;
    struct Token *last = NULL;
    char *saveptr;
    char *str = strtok_r(arg, " ", &saveptr);
    while (str)
    {
        struct Token *token = calloc(1, sizeof(struct Token));

        if (!res)
            res = token;
        else
            last->next = token;

        if (!strcmp("if", str))
            token->type = IF;
        else if (!strcmp("then", str))
            token->type = THEN;
        else if (!strcmp("elif", str))
            token->type = ELIF;
        else if (!strcmp("else", str))
            token->type = ELSE;
        else if (!strcmp("fi", str))
            token->type = FI;
        else if (!strcmp(";", str))
            token->type = SC;
        else if (!strcmp("\n", str))
            token->type = NL;
        else if (!strcmp("'", str))
            token->type = SQ;
        else
        {
            token->type = WORD;
            token->value = calloc(strlen(str) + 1, sizeof(char));
            strcpy(token->value, str);
        }

        last = token;
        str = strtok_r(NULL, " ", &saveptr);
    }

    return res;
}

void print_token(struct Token *token) {
    while (token) {
        switch (token->type) {
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
        default:
            printf("? ");
            break;
        }
    token = token->next;
    }
    printf("\n");
}
