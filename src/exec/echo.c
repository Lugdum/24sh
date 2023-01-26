#include "echo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_redir(char *word)
{
    if (strcmp(word, ">") == 0) // > = 1
        return 1;
    else if (strcmp(word, ">>") == 0) // >> = 2
        return 2;
    else if (strcmp(word, ">|") == 0) // >| = 3
        return 3;
    else if (strcmp(word, ">&") == 0) // >& = 4
        return 4;
    return -1;
}

int echo(struct Node *ast)
{
    int flag_n = 0;
    int flag_e = 0;
    int flag_E = 0;
    int word = 0;
    int son = 0;
    int redir = 0;
    int child_num = ast->nb_children;
    if (child_num > 2)
    {
        redir = is_redir(ast->children[child_num - 2]->value);
        if (redir > 0)
        {
            child_num -= 2;
        }
    }
    while (!word && son < child_num)
    {
        son++;
        char *flag = ast->children[son]->value;
        if (!strcmp(flag, "-e"))
        {
            flag_e = 1;
        }
        else if (!strcmp(flag, "-E"))
        {
            flag_E = 1;
        }
        else if (!strcmp(flag, "-n"))
        {
            flag_n = 1;
        }
        else if (!strcmp(flag, "-en"))
        {
            flag_e = 1;
            flag_n = 1;
        }
        else if (!strcmp(flag, "-ne"))
        {
            flag_e = 1;
            flag_n = 1;
        }
        else if (!strcmp(flag, "-En"))
        {
            flag_E = 1;
            flag_n = 1;
        }
        else if (!strcmp(flag, "-Ee"))
        {
            flag_e = 1;
            flag_E = 1;
        }
        else
            word = 1;
    }

    char *to_print = NULL;
    int len = 0;
    char to_add = 0;
    for (int i = son; i < child_num; i++)
    {
            // Remplace les séquences d'échappement précédées de \ par leurs
            // équivalents ASCII
        char *value = expand_variables_single(ast->children[i]->value);
            for (int j = 0; value[j] != '\0'; j++)
            {
                 
                if (value[j] == '\\' && !flag_E && flag_e)
                {
                    switch (value[j + 1])
                    {
                    case '\\':
                        to_add = '\\';
                        break;
                    case 'n':
                        to_add = '\n';
                        break;
                    case 't':
                        to_add = '\t';
                        break;
                    default:
                        to_add = value[j];
                    }
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
        if (i < child_num - son)
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
    else
    {
        FILE *fp;
        switch (redir)
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
        if (!flag_n)
        {
            if (fputs("\n", fp) == EOF)
            {
                // Couldn't write \n
                return ERROR; 
            }
        }
        if(fclose(fp) == EOF)
        {
            // Couldn't close file
            return ERROR;
        }
    }

    return TRUE;
}
