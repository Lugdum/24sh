#ifndef EXEC_H
#define EXEC_H

#include "echo.h"
#include "../parser/parser.h"
#include "../variable/variable.h"

int main_exec(struct Node *ast);
int node_type(struct Node *ast);

#endif /* !EXEC_H */
