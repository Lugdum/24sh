#define _POSIX_C_SOURCE 200809L

#include "exec.h"
#include "../variable/variable.h"
#include "../variable/special_variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct variable_list *var_list;
char **input_arguments;
int exit_status;

int dot(struct Node *ast)
{
    char *file2 = ast->children[1]->value;
    char *script2 = file_to_char(file2);
    if (script2 == NULL)
        return TRUE;
    struct Token *tokens2 = lexer(script2);
    free(script2);
    struct Node *ast2 = NULL;
    int res = parse(tokens2, &ast2);
    free_lexer(tokens2);
    if (res)
        return ERROR;
    int r = node_type(ast2); 
    free_ast(ast2);
    return r;
}

int process_variable(struct Node *ast)
{
    if (ast->value == NULL)
        return ERROR;
    char *save;
    char *prefix = strtok_r(ast->value, "=", &save);
    if (prefix == NULL)
        return ERROR;
    char *val = strtok_r(NULL, "=\"\'", &save);
    if (val == NULL)
        return ERROR;
    modify_value(var_list, prefix, val);
    return TRUE;
}

int process_function(struct Node *ast)
{
    return node_type(ast->function->body);
}

int exec_command(struct Node *ast)
{
    if (!strcmp(ast->children[0]->value, "echo"))
    {
        return echo(ast);
    }
    if (!strcmp(ast->children[0]->value, "."))
    {
        return dot(ast);
    }
    else if (!strcmp(ast->children[0]->value, "true"))
    {
        return TRUE;
    }
    else if (!strcmp(ast->children[0]->value, "false"))
    {
        return FALSE;
    }
    else if (!strcmp(ast->children[0]->value, "continue"))
    {
        return CONTINUE;
    }
    else if (!strcmp(ast->children[0]->value, "break"))
    {
        return BREAK;
    }
    else if (!strcmp(ast->children[0]->value, "exit"))
    {
        return EXIT;
    }
    else
    {
        return process_variable(ast->children[0]);
    }
    return TRUE;
}

int process_if(struct Node *ast)
{
    if (node_type(ast->children[0]) == TRUE)
        return node_type(ast->children[1]);
    else
    {
        if (ast->nb_children == 3)
            return node_type(ast->children[2]);
        else
            return FALSE;
    }
}

int process_for(struct Node *ast)
{
    int r = TRUE;
    //TODO SUPR afer exit for
    struct Node *tmp = ast->children[0]->children[0]->children[0];
    for (int i = 0; i < tmp->nb_children; i++)
    {
        char **expanded = expand_variables(tmp->children[i]->value);
        int j = 0;
        while (expanded[j])
        {
            modify_value(var_list, ast->value, expanded[j]);
            r = node_type(ast->children[1]);
            //error
            if (r >= ERROR)
            {
                int k = 0;
                while (expanded[k])
                {
                    free(expanded[k]);
                    k++;
                }
                free(expanded);
                return r;
            }
            j++;
        }
        int k = 0;
        while (expanded[k])
        {
            free(expanded[k]);
            k++;
        }
        free(expanded);
    }

    return r;
}

int process_while(struct Node *ast)
{
    int r = TRUE;
    while (node_type(ast->children[0]) == TRUE)
    {
        r = node_type(ast->children[1]);
        if (r == ERROR)
            return r;
        else if (r == BREAK)
            return TRUE;
        else if (r == CONTINUE)
            r = TRUE;
    }
    return r;
}
int process_until(struct Node *ast)
{
    int r = TRUE;
    while (node_type(ast->children[0]) == FALSE)
    {
        r = node_type(ast->children[1]);
        if (r == ERROR)
            return r;
        else if (r == BREAK)
            return TRUE;
        else if (r == CONTINUE)
            r = TRUE;
    }
    return r;
}
int process_cmd(struct Node *ast)
{
    int r = TRUE;
    for (int i = 0; i < ast->nb_children; i++)
    {
        if (ast->children[i]->type == AST_SIMPLE_COMMAND)
        {
            r = node_type(ast->children[i]);
            if (r >= ERROR)
                return r;
        }
    }
    return r;
}

int process_list(struct Node *ast)
{
    int r = TRUE;
    for (int i = 0; i < ast->nb_children; i++)
    {
        r = node_type(ast->children[i]);
        if (r >= ERROR)
            return r;
    }
    return r;
}

int process_pipe(struct Node *ast)
{
    int r = node_type(ast->children[0]);
    if (r >= ERROR)
        return r;
    r = node_type(ast->children[1]);
    exit_status = r;
    return r;
}

int process_and(struct Node *ast)
{
    int l = node_type(ast->children[0]);
    if (l >= ERROR)
        return l;
    int r = node_type(ast->children[1]);
    if (r >= ERROR)
        return r;
    r = ((l == TRUE) && (r == TRUE));
    if (r)
        return TRUE;
    return FALSE;
}

int process_or(struct Node *ast)
{
    int l = node_type(ast->children[0]);
    if (l >= ERROR)
        return l;
    int r = node_type(ast->children[1]);
    if (r >= ERROR)
        return r;
    r = ((l == TRUE) || (r == TRUE));
    if (r)
        return TRUE;
    return FALSE;
}

int process_pipeline(struct Node *ast)
{
    int r = node_type(ast->children[0]);
    if (ast->nb_children == 2)
        r = node_type(ast->children[1]);
    return r;
}
int process_em(struct Node *ast)
{
    int r = node_type(ast->children[0]);
    if (ast->nb_children == 2)
        r = node_type(ast->children[1]);
    if (r == TRUE)
        r = FALSE;
    else if (r == FALSE)
        r = TRUE;
    exit_status = r;
    return r;
}
int node_type(struct Node *ast)
{
    int r = FALSE;
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
    case AST_PIPELINE:
        r = process_pipeline(ast);
        break;
    case AST_EM:
        r = process_em(ast);
        break;
    case AST_BLOCK:
        r = process_list(ast);
        break;
    case AST_CRET_FUNC:
        r = TRUE;
        break;
    case AST_FUNCTION:
        r = process_function(ast);
        break;
    default:
        printf("Unknown Node type\n");
        r = ERROR;
    }
    return r;
}

void comput_special_variables()
{
    char **temp = expand_at();
    modify_value_multiple(var_list, "@", temp);
    int i = 0;
    if (temp)
    {
    while (temp[i])
    {
        free(temp[i]);
        i++;
    }
    free(temp);
    }
    char *tmp;
    for (i = 0; i < 9; i++)
    {
        char buffer[100];
        sprintf(buffer, "%d", i + 1);
        tmp = expand_n(i);
        modify_value(var_list, buffer, tmp);
        free(tmp);
    }
    
    tmp = expand_star();
    modify_value(var_list, "*", tmp);
    free(tmp);
    tmp = expand_question_mark();
    modify_value(var_list, "?", tmp);
    free(tmp);
    tmp = expand_dollar();
    modify_value(var_list, "$", tmp);
    free(tmp);
    tmp = expand_sharp();
    modify_value(var_list, "#", tmp);
    free(tmp);
    tmp = expand_uid();
    modify_value(var_list, "UID", tmp);
    free(tmp);
    tmp = expand_pwd();
    modify_value(var_list, "PWD", tmp);
    free(tmp);

    srand(time(NULL));
}

int main_exec(struct Node *ast, char **input_args)
{
    var_list = calloc(1, sizeof(struct variable_list));
    var_list->size = 0;
    var_list->list = NULL;
    input_arguments = input_args;
    exit_status = 0;    
    
    comput_special_variables();

    int r = node_type(ast);
    if (r != ERROR)
        r = 0;
    free_list(var_list);
    return r;
}
