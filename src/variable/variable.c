#define _POSIX_C_SOURCE 200809L

#include "variable.h"

#include <stdlib.h>
#include <string.h>

#include "../exec/exec.h"
#include "special_variable.h"

char **find_value(struct variable_list *list, char *name)
{
    for (int i = 0; i < list->size; i++)
    {
        if (!strcmp(name, list->list[i].name))
            return list->list[i].value;
    }
    return NULL;
}
void insert_value(struct variable_list *list, char *name, char **value)
{
    list->list =
        realloc(list->list, (1 + list->size) * sizeof(struct variable_list));

    int s = list->size;

    list->list[s].value = NULL;
    list->list[s].name = calloc(strlen(name) + 1, 1);
    list->list[s].name = strcpy(list->list[list->size].name, name);

    int k = 0;
    char *word = value[k];
    while (word)
    {
        list->list[s].value =
            realloc(list->list[s].value, sizeof(char *) * (k + 2));
        list->list[s].value[k] = malloc(strlen(word) + 1);
        list->list[s].value[k] = strcpy(list->list[s].value[k], word);
        k++;
        word = value[k];
    }
    list->list[s].value[k] = NULL;
    list->size += 1;
}
void modify_value(struct variable_list *list, char *name, char *value)
{
    if (value == NULL || value[0] == '\0')
        return;
    char **tmp = malloc(2 * sizeof(char *));
    tmp[0] = value;
    tmp[1] = NULL;
    modify_value_multiple(list, name, tmp);
    free(tmp);
}
void modify_value_multiple(struct variable_list *list, char *name, char **value)
{
    if (value == NULL || value[0] == NULL)
        return;
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
                k = 0;
                char *word = value[k];
                while (word)
                {
                    list->list[i].value =
                        realloc(list->list[i].value, sizeof(char *) * (k + 2));
                    list->list[i].value[k] = malloc(strlen(word) + 1);
                    list->list[i].value[k] =
                        strcpy(list->list[i].value[k], word);
                    k++;
                    word = value[k];
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

char *get_string(char **var)
{
    char *r = NULL;
    int len = 0;
    int k = 0;
    while (var[k])
    {
        r = realloc(r, 1 + len + strlen(var[k]));
        strcpy(r + len, var[k]);
        len += strlen(var[k]);
        r[len] = ' ';
        len++;
        k++;
    }
    r[len - 1] = '\0';
    return r;
}

int *detect_var(int *ii, char *cur, int *is_varr, char *str)
{
    int quot = 0;
    int double_quot = 0;
    int is_var = 0;

    int j = 0;
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\'' && (i == 0 || str[i - 1] != '\''))
        {
            quot = !quot;
        }
        else if (str[i] == '\"' && (i == 0 || str[i - 1] != '\''))
        {
            double_quot = !double_quot;
        }
        else if (!quot && str[i] == '$')
        {
            is_var = 1;
            break;
        }
        else
        {
            cur[j] = str[i];
            j++;
        }
    }
    *ii = i;
    *is_varr = is_var;
    int *tmp = calloc(2, sizeof(int));
    tmp[0] = j;
    tmp[1] = double_quot;
    return tmp;
}

char *get_var_name(int *ii, int *args, char ***var_value, char *str)
{
    int *is_cutted = &args[0];
    int *free_word = &args[1];
    int double_quot = args[2];
    char *word;
    int i = *ii;

    int tmp = i + 1;
    while (str[i] != 0 && str[i] != '\'' && str[i] != '\"')
        i++;
    // if no name
    if (i == tmp)
        word = "$";
    else
    {
        char *tmp_var_name = calloc(i - tmp + 2, 1);
        tmp_var_name = strncpy(tmp_var_name, str + tmp, i - tmp);
        char *tmp_trash;
        char *var_name = strtok_r(tmp_var_name, "{}", &tmp_trash);
        *var_value = find_value(var_list, var_name);

        if (!strcmp(var_name, "RANDOM") || !strcmp(var_name, "(RANDOM)"))
        {
            word = expand_random();
            *free_word = 1;
        }
        else if (!var_value)
        {
            word = "";
        }
        else if (double_quot)
        {
            *free_word = 1;
            word = get_string(*var_value);
        }
        else
        {
            word = "";
            *is_cutted = 1;
        }
        free(tmp_var_name);
    }
    *ii = i;
    return word;
}

void get_if_cutted(char **var_value, char ***rett, int *size_rett)
{
    char **ret = *rett;
    char *word = NULL;
    int size_ret = *size_rett;
    int var_i = 0;
    while (var_value[var_i])
    {
        char *save;
        char *tmp = malloc(strlen(var_value[var_i]) + 1);
        strcpy(tmp, var_value[var_i]);
        word = strtok_r(tmp, " ", &save);
        if (size_ret == 0)
        {
            ret[size_ret] = realloc(ret[size_ret],
                                    strlen(word) + strlen(ret[size_ret]) + 1);
            strcpy(ret[size_ret] + strlen(ret[size_ret]), word);
            word = strtok_r(NULL, " ", &save);
            size_ret += 1;
        }
        while (word)
        {
            ret = realloc(ret, sizeof(char *) * (size_ret + 1));
            ret[size_ret] = malloc(strlen(word) + 1);
            ret[size_ret] = strcpy(ret[size_ret], word);
            word = strtok_r(NULL, " ", &save);
            size_ret++;
        }
        var_i++;
        free(tmp);
    }
    *rett = ret;
    *size_rett = size_ret;
}

char **expand_variables(char *str)
{
    int size_cur = strlen(str) + 1;
    char *cur = calloc(size_cur, 1);

    int size_ret = 0;
    char **ret = NULL;
    int i = 0;
    int is_var = 0;
    int *tmp = detect_var(&i, cur, &is_var, str);
    int j = tmp[0];
    int double_quot = tmp[1];
    free(tmp);
    if (!is_var)
    {
        cur[j] = '\0';
        ret = malloc(sizeof(char *) * 2);
        ret[0] = cur;
        ret[1] = NULL;
        return ret;
    }

    char **var_value;
    // get var name
    int args[3] = { 0, 0, double_quot };
    char *word = get_var_name(&i, args, &var_value, str);
    int free_word = args[1];
    int is_cutted = args[0];
    // copy var value to return string if quoted
    cur = realloc(cur, size_cur + strlen(word));
    strcpy(cur + j, word);

    ret = malloc(sizeof(char *) * 2);
    ret[0] = cur;
    ret[1] = NULL;

    if (free_word)
        free(word);
    if (!is_cutted)
        return ret;

    get_if_cutted(var_value, &ret, &size_ret);

    ret = realloc(ret, sizeof(char *) * (size_ret + 1));
    ret[size_ret] = NULL;
    return ret;
}
char *expand_variables_single(char *str)
{
    char **ret = expand_variables(str);
    char *r = get_string(ret);
    int i = 0;
    while (ret[i])
    {
        free(ret[i]);
        i++;
    }
    free(ret);
    return r;
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
