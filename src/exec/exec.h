#ifndef EXEC_H
#define EXEC_H

#include "echo.h"
#include "../parser/parser.h"
#include "../variable/variable.h"

extern struct variable_list *var_list;
extern char **input_arguments;
extern int exit_status;

int main_exec(struct Node *ast, char **input_args);
int node_type(struct Node *ast);

#endif /* !EXEC_H */
