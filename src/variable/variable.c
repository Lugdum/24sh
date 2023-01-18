#include "variable.h"
#include "../exec/exec.h"

#include <stdlib.h>
#include <string.h>

char *find_value(struct variable_list *list, char *name)
{
    for (int i = 0; i < list->size; i++)
    {
        if (!strcmp(name, list->list[i].name))
            return list->list[i].value;
    }
    return "";
}
void insert_value(struct variable_list *list, char *name, char *value)
{
    list->list = realloc(list->list, list->size * sizeof(struct variable_list));

    list->list[list->size].name = calloc(strlen(name) + 1, 1);
    list->list[list->size].value = calloc(strlen(value) + 1, 1);

    list->list[list->size].name = strcpy(list->list[list->size].name, name);
    list->list[list->size].value = strcpy(list->list[list->size].value, value);

    list->size += 1;
}
void modify_value(struct variable_list *list, char *name, char *value)
{
    if (!strcmp("", find_value(list, name)) && !strcmp("", value))
    {
    for (int i = 0; i < list->size; i++)
    {
        if (!strcmp(name, list->list[i].name))
        {
            list->list[i].value = realloc(list->list[i].value, strlen(value) + 1);
            list->list[i].value = strcpy(list->list[i].value, value);
        }
    }
    }
    else
    {
        insert_value(list, name, value);
    }
}
void free_list(struct variable_list *list)
{
    for (int i = 0; i < list->size; i++)
    {
        free(list->list[i].value);
        free(list->list[i].name);
    }
    free(list->list);
    free(list);
}

char *expand_at()
{
    int i = 0;
    char *r = calloc(1, 1);
    int size = 0;
    while (input_arguments[i] != 0)
    {
        r = realloc(r, size + 1 + strlen(input_arguments[i]));
        r[size] = ' ';
        strcpy(r + size + 1, input_arguments[i]);
        size += 1 + strlen(input_arguments[i]);
        i++;
    }
    return r;
}



char *expand_variables(char *str)
{
    int size = strlen(str);
    char *r = calloc(size, 1);
    int quot = 0;
    int j = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\'' && (i == 0 || str[i - 1] != '\''))
        {
            quot = !quot;
        }
        else if (!quot && str[i] == '$')
        {
            int tmp = i + 1;
            int free_word = 1;
            char *word;
            //get var name
            while (str[i] != 0 && str[i] != ' ' && str[i] != '\'')
                i++;
            //if no name
            if (i == tmp)
                word ="$";
            else
            {
                char *var_name = calloc(i - tmp + 2, 1);
                var_name = strncpy(var_name, str + tmp, i - tmp);
                else
                {
                    word = find_value(var_list, var_name);
                    free_word = 0;
                }
                free(var_name);
            }

            //copy var value to return string
            r = realloc(r, size + strlen(word));
            strcpy(r + j, word);
            j += strlen(word);
            if (free_word)
                free(word);
        }
        else
        {
            r[j] = str[i];
            j++;
        }

    }
    return r;
}
