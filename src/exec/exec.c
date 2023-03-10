#include "exec.h"

char **input_arguments;

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
    return r;
}
int other_node_type(struct Node *ast)
{
    int r = 0;
    switch (ast->type)
    {
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
    default:
        return other_node_type(ast);
    }
    return r;
}

void comput_special_variables(void)
{
    char **temp = expand_at();
    modify_value_multiple("@", temp);
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
        modify_value(buffer, tmp);
        free(tmp);
    }

    tmp = expand_star();
    modify_value("*", tmp);
    free(tmp);
    tmp = expand_question_mark();
    modify_value("?", tmp);
    free(tmp);
    tmp = expand_dollar();
    modify_value("$", tmp);
    free(tmp);
    tmp = expand_sharp();
    modify_value("#", tmp);
    free(tmp);
    tmp = expand_uid();
    modify_value("UID", tmp);
    free(tmp);
    tmp = expand_pwd();
    modify_value("PWD", tmp);
    free(tmp);

    srand(time(NULL));
}

int main_exec(struct Node *ast, char **input_args)
{

    define_var_list();
    input_arguments = input_args;

    comput_special_variables();

    int r = node_type(ast);
    if (r != ERROR)
        r = 0;
    free_list(var_list);
    return r;
}
