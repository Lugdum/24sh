#include "echo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_redir(char *word)
{
    // > = 1
    // >> = 2
    // >| = 1 cause we don't have the noclobber option
    // >& = 3
    if (strcmp(word, ">") == 0)
        return 1;
    else if (strcmp(word, ">>") == 0)
        return 2;
    else if (strcmp(word, ">|") == 0)
        return 1;
    else if (strcmp(word, ">&") == 0)
        return 3;
    return -1;
}

void print_to_stdout(int len, char *to_print, int flag_n)
{
    if (len > 0)
    {
        if (to_print[len - 1] == ' ')
            to_print[len - 1] = '\0';
        else
        {
            to_print = realloc(to_print, len + 1);
            to_print[len] = '\0';
        }
        printf("%s", to_print);
        free(to_print);
    }

    if (!flag_n)
    {
        putchar('\n');
    }
}

int print_to_file(int *redir_and_flag_n, struct Node *ast, int len,
                  char *to_print)
{
    FILE *fp;
    switch (redir_and_flag_n[0])
    {
    case 1:
        fp = fopen(ast->children[ast->nb_children - 1]->value, "w");
        break;
    case 2:
        fp = fopen(ast->children[ast->nb_children - 1]->value, "a");
        break;
    default:
        fp = NULL;
        break;
    }
    if (fp == NULL)
    {
        // The file could not be opened or created.
        return ERROR;
    }
    if (len > 0)
    {
        if (to_print[len - 1] == ' ')
            to_print[len - 1] = '\0';
        else
        {
            to_print = realloc(to_print, len + 1);
            to_print[len] = '\0';
        }
        if (fprintf(fp, "%s", to_print) < 0)
        {
            // Couldn't write string in file
            return ERROR;
        }
        free(to_print);
    }
    if (!redir_and_flag_n[1])
    {
        if (fputs("\n", fp) == EOF)
        {
            // Couldn't write \n
            return ERROR;
        }
    }
    if (fclose(fp) == EOF)
    {
        // Couldn't close file
        return ERROR;
    }
    return TRUE;
}

void fill_flags(int *word_and_child_num, int *son, int **flags,
                struct Node *ast)
{
    while (!word_and_child_num[0] && *son < word_and_child_num[1])
    {
        *son += 1;
        char *flag = ast->children[*son]->value;
        if (!strcmp(flag, "-e"))
        {
            *flags[1] = 1;
        }
        else if (!strcmp(flag, "-E"))
        {
            *flags[2] = 1;
        }
        else if (!strcmp(flag, "-n"))
        {
            *flags[0] = 1;
        }
        else if (!strcmp(flag, "-en"))
        {
            *flags[1] = 1;
            *flags[0] = 1;
        }
        else if (!strcmp(flag, "-ne"))
        {
            *flags[1] = 1;
            *flags[0] = 1;
        }
        else if (!strcmp(flag, "-En"))
        {
            *flags[2] = 1;
            *flags[0] = 1;
        }
        else if (!strcmp(flag, "-Ee"))
        {
            *flags[1] = 1;
            *flags[2] = 1;
        }
        else
            word_and_child_num[0] = 1;
    }
}

char switch_replace(char *value, int j)
{
    switch (value[j + 1])
    {
    case '\\':
        return '\\';
        break;
    case 'n':
        return '\n';
    case 't':
        return '\t';
    default:
        return value[j];
    }
}

int echo(struct Node *ast)
{
    int son = 0;
    int redir = 0;
    int child_num = ast->nb_children;
    if (child_num > 2)
    {
        redir = is_redir(ast->children[child_num - 2]->value);
        if (redir > 0)
            child_num -= 2;
    }
    // order flags: flag_n, flag_e, flag_E
    int *flags = calloc(3, sizeof(int));
    // order: word, child_num
    int word_and_child_num[2] = { 0, child_num };
    fill_flags(word_and_child_num, &son, &flags, ast);
    char *to_print = NULL;
    int len = 0;
    char to_add = 0;
    for (int i = son; i < child_num; i++)
    {
        // Remplace les séquences d'échappement précédées de \ par leurs
        // équivalents ASCII
        char *value = expand_variables_single(ast->children[i]->value);
        for (int j = 0; value && value[j] != '\0'; j++)
        {
            if (value[j] == '\\' && !flags[2] && flags[1])
            {
                to_add = switch_replace(value, j);
                j++;
            }
            else
            {
                to_add = value[j];
            }
            to_print = realloc(to_print, len + 1);
            to_print[len] = to_add;
            to_add = 0;
            len++;
        }
        // Ajoute un espace entre chaque argument, sauf pour le dernier
        if (value && i < child_num - son)
        {
            to_print = realloc(to_print, len + 1);
            to_print[len] = ' ';
            to_add = 0;
            len++;
        }
        free(value);
    }
    if (ast->nb_children == child_num)
    {
        print_to_stdout(len, to_print, flags[0]);
    }
    else
    {
        int list[2] = { redir, flags[0] };
        if (!print_to_file(list, ast, len, to_print))
            return ERROR;
    }
    free(flags);
    return TRUE;
}
