#ifndef VARIABLE_H
#define VARIABLE_H

extern struct variable_list *var_list;

struct variable_list
{
    int size;
    struct variable *list;
};

struct variable
{
    char *name;
    char **value;
};

char **find_value(struct variable_list *list, char *name);
void modify_value(char *name, char *value);
void modify_value_multiple(char *name, char **value);
char **expand_variables(char *str);
char *expand_variables_single(char *str);
#endif /* !VARIABLE_H */
