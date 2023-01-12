#include "echo.h"

#include <stdio.h>
#include <string.h>

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
            for (int j = 0; ast->children[i]->value[j] != '\0'; j++)
            {
                if (ast->children[i]->value[j] == '$')
                {
                    char *var = ast->children[i]->value + 1;
                    char *word = find_value(var_list, var);
                    printf("%s", word);
                    j = strlen(ast->children[i]->value) - 1;
                    continue;
                }
                if (ast->children[i]->value[j] == '\\' && !flag_E && flag_e)
                {
                    switch (ast->children[i]->value[j + 1])
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
                        putchar(ast->children[i]->value[j]);
                    }
                    j++;
                }
                else
                {
                    putchar(ast->children[i]->value[j]);
                }
            }

        // Ajoute un espace entre chaque argument, sauf pour le dernier
        if (i < ast->nb_children - son)
        {
            putchar(' ');
        }
    }

    if (!flag_n)
    {
        putchar('\n');
    }
    return 1;
}
