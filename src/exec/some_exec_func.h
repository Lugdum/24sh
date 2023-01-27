#ifndef SOME_EXEC_FUNC_H
#define SOME_EXEC_FUNC_H

#define _POSIX_C_SOURCE 200809L

#include "exec.h"
#include "../variable/variable.h"
#include "../parser/parser.h"
#include "../variable/special_variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "echo.h"

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

