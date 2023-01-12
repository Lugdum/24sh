#include "variable.h"

#include <stdlib.h>
#include <string.h>

char *find_value(struct variable_list *list, char *name)
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

    list->list[list->size].name = calloc(strlen(name) + 1, 1);
    list->list[list->size].value = calloc(strlen(value) + 1, 1);

    list->list[list->size].name = strcpy(list->list[list->size].name, name);
    list->list[list->size].value = strcpy(list->list[list->size].value, value);

    list->size += 1;
}
void modify_value(struct variable_list *list, char *name, char *value)
{
    if (find_value(list, name) != NULL)
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
