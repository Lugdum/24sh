#include "variable.h"
#include "special_variable.h"
#include "../exec/exec.h"

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

char *expand_star()
{
    if (!input_arguments)
        return NULL;
    int i = 0;
    char *r = calloc(1, 1);
    int size = 0;
    while (input_arguments[i] != 0)
    {
        r = realloc(r, size + 1 + strlen(input_arguments[i]));
        strcpy(r + size, input_arguments[i]);
        size += strlen(input_arguments[i]);
        r[size] = ' ';
        size += 1;
        i++;
    }
    if (size > 0)
        r[size - 1] = '\0';
    return r;
}

char **expand_at()
{
    if (!input_arguments)
        return NULL;
    int i = 0;
    char **r = NULL;
    int size = 0;
    while (input_arguments[i] != 0)
    {
        r = realloc(r, sizeof(char *) * (size + 1)); 
        r[size] = malloc(strlen(input_arguments[i]) + 1);
        r[size] = strcpy(r[i], input_arguments[i]);
        i++;
        size++;
    }
    r = realloc(r, sizeof(char *) * (size + 1)); 
    r[size] = NULL;
    return r;
}
char *expand_question_mark()
{
    char *b = calloc(100, 1);
    sprintf(b, "%d", exit_status);
    return b;
}

char *expand_dollar()
{
    char *b = calloc(100, 1);
    sprintf(b, "%i", getpid());
    return b;
}

char *expand_n(int n)
{
    if (!input_arguments)
        return NULL;
    char *r = NULL;
    int size = 0;
    int i = 0;
    while (input_arguments[i] != NULL && i != n)
        i++;
    if (i == n && input_arguments[i] != NULL)
    {
        r = realloc(r, size + 1 + strlen(input_arguments[n]));
        strcpy(r, input_arguments[n]);
    }
    return r;
}

char *expand_sharp()
{
    if (!input_arguments)
        return NULL;
    char *b = calloc(100, 1);
    int j = 0;
    while (input_arguments[j] != NULL)
        j++;
    sprintf(b, "%d", j);
    return b;
}

char *expand_uid()
{
    char *b = calloc(100, 1);
    sprintf(b, "%d", getuid());
    return b;
}

char *expand_pwd()
{
    char *b = calloc(1000, 1);
    getcwd(b, 1000);
    return b;
}

char *expand_random()
{
    int r = rand() % 32767;
    char *b = calloc(10, 1);
    sprintf(b, "%d", r);
    return b;
}
