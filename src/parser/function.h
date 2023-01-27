#ifndef FUNCTION_H
#define FUNCTION_H

#include "../lexer/lexer.h"

extern struct Function *fonctions;

struct Function
{
    char *name;
    struct Node *body;
    int ref_count;
    struct Function *next;
};

int parseFunction(struct Token **token, char *name);
int parseFunctionReplace(struct Token **token, struct Function *replace);
struct Function *findFunction(const char *name);
void free_functions(struct Function *func);
void printFunction(void);

#endif /* !FUNCTION_H */