#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int exec_command(struct Node *ast)
{
    if (!strcmp(ast->children[0]->value, "echo"))
    {
        return echo(ast, 1);
    }
    if (!strcmp(ast->children[0]->value, "true"))
    {
        return 1;
    }
    if (!strcmp(ast->children[0]->value, "false"))
    {
        return 0;
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
   /*     if (strcmp(tmp->children[i]->value, ".."))
        {*/
            //if not..
            modify_value(var_list, ast->value, tmp->children[i]->value);
            r = node_type(ast->children[1]);
      /*  }
        else
        {
            char *end;
            if (tmp->nb_children <= i + 1)
            {
                //last char ..
                fprintf( stderr, "invalid for loop condition");
                return 2;
            }
            long int_begin = strtol(tmp->children[i - 1]->value, &end, 10);
            if (!strcmp(tmp->children[i]->value, end))
            {
                //letter in nuber
                fprintf( stderr, "invalid chars in for loop condition begin\n");
                return 2;
            }
            long int_end = strtol(tmp->children[i + 1]->value, &end, 10);
            if (!strcmp(tmp->children[i]->value, end))
            {
                //letter in nuber
                fprintf( stderr, "invalid chars in for loop condition end\n");
                return 2;
            }
            if (int_begin < int_end)
            {
                int_begin++;
            for (; int_begin < int_end; int_begin ++)
            {
                char *buffer = calloc(101, 1);
                sprintf(buffer, "%ld", int_begin);
                modify_value(var_list, ast->value, buffer);
                r = node_type(ast->children[1]);
            }
            }
            else
            {
                int_begin--;
            for (; int_begin > int_end; int_begin --)
            {
                char *buffer = calloc(101, 1);
                sprintf(buffer, "%ld", int_begin);
                modify_value(var_list, ast->value, buffer);
                r = node_type(ast->children[1]);
            }
            }
            break;
        }*/
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
