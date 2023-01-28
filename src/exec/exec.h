#ifndef EXEC_H
#define EXEC_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../parser/parser.h"
#include "../variable/special_variable.h"
#include "../variable/variable.h"
#include "echo.h"
#include "some_exec_func.h"

extern int exit_status;

int main_exec(struct Node *ast, char **input_args);
int node_type(struct Node *ast);

#define TRUE 0
#define FALSE 1
#define ERROR 2
#define CONTINUE 3
#define BREAK 4
#define EXIT 5

#endif /* !EXEC_H */
