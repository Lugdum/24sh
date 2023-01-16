#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct variable_list *var_list;

int process_variable(struct Node *ast)
{
    if (ast->value == NULL)
        return 2;
    char *save;
    char *prefix = strtok_r(ast->value, "= ", &save);
    if (prefix == NULL)
        return 2;
    char *val = strtok(NULL, "= ", &save);
    if (val == NULL)
        return 2;
    modify_value(var_list, prefix, val);
    return 1;
}

int exec_command(struct Node *ast)
{
    if (!strcmp(ast->children[0]->value, "echo"))
    {
        return echo(ast, 1);
    }
    else if (!strcmp(ast->children[0]->value, "true"))
    {
        return 1;
    }
    else if (!strcmp(ast->children[0]->value, "false"))
    {
        return 0;
    }
    else
    {
        return process_variable(ast->children[0]);
    }

    return 1;
}

int process_if(struct Node *ast)
{
    if (node_type(ast->children[0]))
        return node_type(ast->children[1]);
    else
        return node_type(ast->children[2]);
}

int process_for(struct Node *ast)
{
    int r = 1;
    //TODO SUPR afer exit for
    struct Node *tmp = ast->children[0]->children[0];
    for (int i = 0; i < tmp->nb_children; i++)
    {
        modify_value(var_list, ast->value, tmp->children[i]->value);
        r = node_type(ast->children[1]);
        if (r == 2)
            return 2;
    }
    return r;
}

int process_while(struct Node *ast)
{
    int r = 1;
    while (node_type(ast->children[0]))
    {
        r = node_type(ast->children[1]);
        if (r == 2)
            return 2;
    }
    return r;
}
int process_until(struct Node *ast)
{
    int r = 1;
    while (!node_type(ast->children[0]))
    {
        r = node_type(ast->children[1]);
        if (r == 2)
            return 2;
    }
    return r;
}
int process_cmd(struct Node *ast)
{
    int r = 1;
    for (int i = 0; i < ast->nb_children; i++)
    {
        if (ast->children[i]->type == AST_SIMPLE_COMMAND)
        {
            r = node_type(ast->children[i]);
            if (r == 2)
                return 2;
        }
    }
    return r;
}

int process_list(struct Node *ast)
{
    int r = 0;
    for (int i = 0; i < ast->nb_children; i++)
    {
        r = node_type(ast->children[i]);
        if (r == 2)
            return 2;
    }
    return r;
}

int process_pipe(struct Node *ast)
{
    // int fd = -1;
    int r = node_type(ast->children[0]);
    if (r == 2)
        return 2;
    r = node_type(ast->children[1]);
    return r;
}

int process_and(struct Node *ast)
{
    int l = node_type(ast->children[0]);
    if (l == 2)
        return 2;
    int r = node_type(ast->children[1]);
    if (r == 2)
        return 2;
    return l && r;
}

int process_or(struct Node *ast)
{
    int l = node_type(ast->children[0]);
    if (l == 2)
        return 2;
    int r = node_type(ast->children[1]);
    if (r == 2)
        return 2;
    return l || r;
}

int node_type(struct Node *ast)
{
    int r = 0;
    switch (ast->type)
    {
    case AST_INPUT:
        r = process_cmd(ast);
        break;
    case AST_LIST:
        r = process_list(ast);
        break;
    case AST_AND:
        r = process_and(ast);
        break;
    case AST_OR:
        r = process_or(ast);
        break;
    case AST_FOR:
        r = process_for(ast);
        break;
    case AST_COMMAND:
        r = process_cmd(ast);
        break;
    case AST_WHILE:
        r = process_while(ast);
        break;
    case AST_UNTIL:
        r = process_until(ast);
        break;
    case AST_SIMPLE_COMMAND:
        r = exec_command(ast);
        break;
    case AST_IF:
        r = process_if(ast);
        break;
    default:
        printf("Unknown Node type\n");
        r = 2;
    }
    return r;
}

int main_exec(struct Node *ast)
{
    var_list = calloc(1, sizeof(struct variable_list));
    var_list->size = 0;
    var_list->list = NULL;
    return node_type(ast);
}
