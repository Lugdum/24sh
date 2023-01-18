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
                    case 'a':
                        putchar('\a');
                        break;
                    case 'b':
                        putchar('\b');
                        break;
                    case 'f':
                        putchar('\f');
                        break;
                    case 'n':
                        putchar('\n');
                        break;
                    case 'r':
                        putchar('\r');
                        break;
                    case 't':
                        putchar('\t');
                        break;
                    case 'v':
                        putchar('\v');
                        break;
                    default:
                        putchar(value[j]);
                    }
                    j++;
                }
                else
                {
                    putchar(value[j]);
                }
            }

        // Ajoute un espace entre chaque argument, sauf pour le dernier
        if (i < ast->nb_children - son)
        {
            putchar(' ');
        }
        free(value);
    }

    if (!flag_n)
    {
        putchar('\n');
    }
    return 1;
}
