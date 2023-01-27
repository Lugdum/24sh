#define _POSIX_C_SOURCE 200809L
#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Token *no_double(struct Token *tok)
{
    struct Token *pre = tok;
    struct Token *begin = tok;
    while (tok->type == SC)
    {
        pre = tok;
        tok = tok->next;
        free(pre->value);
        free(pre);
    }
    begin = tok;
    pre = tok;
    tok = tok->next;
    while (tok)
    {
        if (tok->type == SC && (pre->type == SC || pre->type == B_OP))
        {
            struct Token *tmp = tok->next;
            pre->next = tok->next;

            free(tok->value);
            free(tok);

            tok = tmp;
        }
        else
        {
            pre = tok;
            tok = tok->next;
        }
    }
    return begin;
}

void fix_chevre(struct Token *tok)
{
    struct Token *pre = tok;
    tok = tok->next;
    while (tok)
    {
        if (pre->type == CHEVRER)
        {
            pre->type = WORD;
            pre->value = calloc(4, 1);
            int rm = 1;
            if (tok->type == CHEVRER)
                strcpy(pre->value, ">>\0");
            else if (tok->type == ESP)
                strcpy(pre->value, ">&\0");
            else if (tok->type == PIPE)
                strcpy(pre->value, ">|\0");
            else
            {
                strcpy(pre->value, ">\0");
                rm = 0;
            }
            if (rm)
            {
                pre->next = tok->next;
                free(tok->value);
                free(tok);
                tok = pre->next;
            }
        }
        else if (pre->type == CHEVREL)
        {
            pre->type = WORD;
            pre->value = calloc(4, 1);
            int rm = 1;
            if (tok->type == CHEVRER)
                strcpy(pre->value, "<>\0");
            else if (tok->type == ESP)
                strcpy(pre->value, "<&\0");
            else
            {
                strcpy(pre->value, ">\0");
                rm = 0;
            }
            if (rm)
            {
                pre->next = tok->next;
                free(tok->value);
                free(tok);
                tok = pre->next;
            }
        }
        else if (pre->type == ESP && tok->type == ESP)
        {
            pre->next = tok->next;
            free(tok->value);
            free(tok);
            tok = pre->next;
            pre->type = AND;
        }
        else if (pre->type == PIPE && tok->type == PIPE)
        {
            pre->next = tok->next;
            free(tok->value);
            free(tok);
            tok = pre->next;
            pre->type = OR;
        }
        pre = tok;
        tok = tok->next;
    }
}

struct Token *process_end_of_file(struct Token *tok)
{
    struct Token *token = calloc(1, sizeof(struct Token));
    token->value = NULL;
    token->next = NULL;

    token->type = EF;

    tok->next = token;
    return token;
}

int other_types(char *str, struct Token *token)
{
    int r = 1;
    if (!strcmp("then", str))
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
    else if (!strcmp(",", str))
        token->type = VIRG;
    else if (!strcmp("!", str))
        token->type = EM;
    else if (!strcmp("&", str))
        token->type = ESP;
    else if (!strcmp("(", str))
        token->type = PL;
    else if (!strcmp(")", str))
        token->type = PR;
    else if (!strcmp("case", str))
        token->type = CASE;
    else if (!strcmp("esac", str))
        token->type = ESAC;
    else if (!strcmp("{", str))
        token->type = B_OP;
    else if (!strcmp("}", str))
        token->type = B_CL;
    else if (!strcmp("\n", str))
        token->type = NL;
    else if (!strcmp("'", str))
        token->type = SQ;
    else
        r = 0;
    return r;
}

int other_other_types(char *str, struct Token *token)
{
    int r = 1;
    if (!strcmp("if", str))
        token->type = IF;
    else if (!strcmp("||", str))
        token->type = OR;
    else if (!strcmp("for", str))
        token->type = FOR;
    else if (!strcmp("in", str))
        token->type = IN;
    else if (!strcmp("do", str))
        token->type = DO;
    else if (!strcmp(">", str))
        token->type = CHEVRER;
    else if (!strcmp("<", str))
        token->type = CHEVREL;
    else if (!strcmp("done", str))
        token->type = DONE;
    else if (!strcmp("while", str))
        token->type = WHILE;
    else if (!strcmp("until", str))
        token->type = UNTIL;
    else if (!strcmp("&&", str))
        token->type = AND;
    else
        r = 0;
    return r;
}

struct Token *process(char *str, struct Token *tok)
{
    struct Token *token = calloc(1, sizeof(struct Token));
    token->value = NULL;
    token->next = NULL;
    int r = 1;

    if (strlen(str) == 0)
    {
        free(token);
        return tok;
    }
    else if (!strcmp(" ", str))
    {
        free(token);
        return tok;
    }
    else 
        r = other_types(str, token);
    if (!r)
        r = other_other_types(str, token);
    if (!r)
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

int is_in(char c, char *str)
{
    int i = 0;
    while (str[i])
    {
        if (c == str[i])
            return 1;
        i++;
    }
    return 0;
}

struct Token *lexer(char *input)
{
    // printf("%s\n", input);
    struct Token *out = calloc(1, sizeof(struct Token));
    struct Token *cur_tok = out;

    int len = strlen(input);
    char *cur = calloc(len + 1, 1);
    int j = 0;
    bool quote = false;
    int i = 0;
    char term[] = { ' ', ';', '\n', ',', '|', '&', '>', '<'};
    while (i < len)
    {
        // if # at the start of line then whole line is comment
        if (input[i] == '#' && (i == 0 || input[i]))
        {
            while (input[i] != '\n')
                i++;
        }
        // if " but no \ then start/end quoting
        if ((input[i] == '\"' || input[i] == '\'') && input[i - 1] != '\\')
        {
            quote = !quote;
            cur[j] = input[i];
            j++;
        }
        // if space ; or \n then end token exept if quoted
        if (is_in(input[i], term) && !quote)
        {
            cur[j] = '\0';
            cur_tok = process(cur, cur_tok);
            cur[0] = input[i];
            if (input[i] == '\n')
                cur[0] = ';';
            cur[1] = '\0';
            cur_tok = process(cur, cur_tok);
            j = 0;
        }
        // if \ or \r then skip the char
        else if (!((input[i] == '\\' && !quote) || input[i] == '\r'))
        {
            cur[j] = input[i];
            j++;
        }
        i++;
    }
    cur[j] = '\0';
    cur_tok = process(cur, cur_tok);
    cur_tok = process_end_of_file(cur_tok);

    free(cur);
    cur_tok = out->next;
    free(out);
    cur_tok = no_double(cur_tok);
    fix_chevre(cur_tok);
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
        case CASE:
            printf("CASE ");
            break;
        case ESAC:
            printf("ESAC ");
            break;
        case VIRG:
            printf(", ");
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
        case EM:
            printf("! ");
            break;
        case B_OP:
            printf("{ ");
            break;
        case PL:
            printf("( ");
            break;
        case PR:
            printf(") ");
            break;
        case B_CL:
            printf("} ");
            break;
        case CHEVRER:
            printf("> ");
            break;
        case CHEVREL:
            printf("< ");
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

char *file_to_char(char *file)
{
    char *script = malloc(sizeof(char));
    char *line = NULL;
    size_t n = 1;
    size_t len = 0;
    // ssize_t getline();
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "An error occured when openning %s\n", file);
        free(script);
        return NULL;
    }
    while (getline(&line, &len, fp) != -1)
    {
        size_t i = 0;
        len = strlen(line);
        script = realloc(script, sizeof(char) * (n + len));
        while (i < len)
        {
            // I need the \n for the comments
            if (line[i] == '\n')
            {
                script[n - 1] = '\n';
                i++;
                n++;
                continue;
            }
            script[n - 1] = line[i];
            i++;
            n++;
        }
    }
    if (line)
        free(line);
    fclose(fp);
    script[n - 1] = '\0';
    return script;
}
