#ifndef EXEC_H
#define EXEC_H

#include "echo.h"
#include "../parser/parser.h"
#include "../variable/variable.h"

#ifndef VARLIST
#define VARLIST
extern struct variable_list *var_list;
#endif

int main_exec(struct Node *ast);
int node_type(struct Node *ast);

#endif /* !EXEC_H */
