#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "echo.h"

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

int process_cmd(struct Node *ast)
{
    int r = 1;
    for (int i = 0; i < ast->nb_children; i++)
    {
        if (ast->children[i]->type == AST_SIMPLE_COMMAND)
            r = node_type(ast->children[i]);
    }
    return r;
}

int process_list(struct Node *ast)
{
    int r = 0;
    for (int i = 0; i < ast->nb_children; i++)
        r = node_type(ast->children[i]);
    return r;
}

int process_pipe(struct Node *ast)
{
    // int fd = -1;
    node_type(ast->children[0]);
    return node_type(ast->children[1]);
}

int process_and(struct Node *ast)
{
    return node_type(ast->children[0]) && node_type(ast->children[1]);
}

int process_or(struct Node *ast)
{
    return node_type(ast->children[0]) || node_type(ast->children[1]);
}

int node_type(struct Node *ast)
{
    switch (ast->type)
    {
    case AST_INPUT:
        return process_cmd(ast);
    case AST_LIST:
        return process_list(ast);
    case AST_PIPELINE:
        // TODO
        break;
    case AST_AND:
        return process_and(ast);
    case AST_OR:
        return process_or(ast);
    case AST_COMMAND:
        return process_cmd(ast);
    case AST_SIMPLE_COMMAND:
        return exec_command(ast);
    case AST_ELEMENT:
        // TODO
        break;
    case AST_CMD:
        // TODO
        break;
    case AST_IF:
        return process_if(ast);
    default:
        printf("Unknown Node type\n");
        return 2;
    }
    return 0;
}
