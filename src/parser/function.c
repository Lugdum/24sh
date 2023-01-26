#include "function.h"
#include "parser.h"
#include "parser_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Function *functions;

int parseFunction(struct Token **token, char *name)
{
    //skip NL
    while ((*token) != NULL && (*token)->type != B_OP)
        (*token) = (*token)->next;
    if (*token == NULL)
        return 2;

    struct Function *new_func = calloc(1, sizeof(struct Function));
    new_func->name = calloc(1, strlen(name) + 1);
    strcpy(new_func->name, name);
    new_func->ref_count = 1;
    if(parseBlockCommand(token, &(new_func->body)))
        return 2;

    new_func->next = functions;
    functions = new_func;
    return 0;
}


struct Function* findFunction(const char* name)
{
    struct Function *current = functions;
    while (current)
    {
        if (!strcmp(current->name, name))
            return current;
        current = current->next;
    }
    return NULL;
}


void printFunction(void)
{
    struct Function *current = functions;
    while (current)
    {
        prettyprint(current->body, stdout);
        current = current->next;
    }
}


void free_functions(struct Function *func)
{
    struct Function *next;
    while (func != NULL)
    {
        next = func->next;
        free(func->name);
        free_ast(func->body);
        free(func);
        func = next;
    }
}
