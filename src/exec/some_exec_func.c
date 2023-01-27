#include "some_exec_func.h"

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
