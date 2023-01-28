#ifndef SOME_EXEC_FUNC_H
#define SOME_EXEC_FUNC_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../parser/parser.h"
#include "../variable/special_variable.h"
#include "../variable/variable.h"
#include "echo.h"
#include "exec.h"

extern char **input_arguments;

int dot(struct Node *ast);
int process_variable(struct Node *ast);
int process_function(struct Node *ast);
int exec_command(struct Node *ast);
int process_if(struct Node *ast);
int process_for(struct Node *ast);
int process_while(struct Node *ast);
int process_until(struct Node *ast);
int process_cmd(struct Node *ast);

#endif /* ! SOME_EXEC_FUNC_H */
