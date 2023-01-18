#include "variable.h"
#include "../exec/exec.h"

#include <stdlib.h>
#include <string.h>

char **find_value(struct variable_list *list, char *name)
{
    for (int i = 0; i < list->size; i++)
    {
        if (!strcmp(name, list->list[i].name))
            return list->list[i].value;
    }
    return NULL;
}
void insert_value(struct variable_list *list, char *name, char *value)
{
    list->list = realloc(list->list, list->size * sizeof(struct variable_list));

    int s = list->size;

    list->list[s].name = calloc(strlen(name) + 1, 1);
    list->list[s].name = strcpy(list->list[list->size].name, name);

    char *save;
    char *word = strtok_r(value, " ", &save);
    int k = 0;
    while (word)
    {
        list->list[s].value = realloc(list->list[s].value, sizeof(char *) * (k + 2));
        list->list[s].value[k] = malloc(strlen(word) + 1);
        list->list[s].value[k] = strcpy(list->list[s].value[k], word);
        k++;
        word = strtok_r(NULL, " ", &save);
    }
    list->list[s].value[k] = NULL;
    list->size += 1;
}
void modify_value(struct variable_list *list, char *name, char *value)
{
    if (find_value(list, name))
    {
    for (int i = 0; i < list->size; i++)
    {
        if (!strcmp(name, list->list[i].name))
        {
            int k = 0;
            while (list->list[i].value[k])
            {
                free(list->list[i].value[k]);
                k++;
            }
            char *save;
            char *word = strtok_r(value, " ", &save);
            k = 0;
            while (word)
            {
                list->list[i].value = realloc(list->list[i].value, sizeof(char *) * (k + 2));
                list->list[i].value[k] = malloc(strlen(word) + 1);
                list->list[i].value[k] = strcpy(list->list[i].value[k], word);
                k++;
                word = strtok_r(NULL, " ", &save);
            }
            list->list[i].value[k] = NULL;
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
        int k = 0;
        while (list->list[i].value[k])
        {
            free(list->list[i].value[k]);
            k++;
        }
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

char *get_string(char **var)
{
    char *r = NULL;
    int len = 1;
    int k = 0;
    while (var[k])
    {
        r = realloc(r, len + strlen(var[k]));
        strcpy(r + len - 1, var[k]);
        len += strlen(var[k]);
        k++;
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
            char *word;
            int free_word = 0;
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
                
                char **var_value = find_value(var_list, var_name);
                if (var_value)
                {
                    word = get_string(var_value);
                    free_word = 1;
                }
                else
                    word = "";
                
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
