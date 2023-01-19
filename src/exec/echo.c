#include "echo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int echo(struct Node *ast, int fd)
{
    int flag_n = 0;
    int flag_e = 0;
    int flag_E = 0;
    int word = 0;
    int son = 0;
    while (!word && son < ast->nb_children)
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
    for (int i = son; i < ast->nb_children; i++)
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
        if (i < ast->nb_children - son)
        {
                to_print = realloc(to_print, len + 1);
                to_print[len] = ' ';
                to_add = 0;
                len++;
        }
        free(value);
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
        printf("%s", to_print);
        free(to_print);
    }

    if (!flag_n)
    {
        putchar('\n');
    }
    return 1;
}
