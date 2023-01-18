#include "variable.h"
#include "special_variable.h"

#include <sys/types.h>
#include <string.h>
#include <unistd.h>

char *expand_star()
{
    int i = 0;
    char *r = calloc(1, 1);
    int size = 0;
    while (input_arguments[i] != 0)
    {
        r = realloc(r, size + 1 + strlen(input_arguments[i]));
        r[size] = ' ';
        strcpy(r + size + 1, input_arguments[i]);
        size += 1 + strlen(input_arguments[i]);
        i++;
    }
    r = realloc(r, size + 1);
    r[size] = '\0';
    return r;
}

char **expand_at()
{
    int i = 0;
    char **r = NULL;
    int size = 0;
    while (input_arguments[i] != 0)
    {
        r = realloc(r, sizeof(char *) * (size + 1)); 
        r[size] = malloc(sizeof(input_arguments[i]) + 1);
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
    sprintf(b, "%lu", get_pid());
    return b;
}

char *expand_n(int n)
{
    char *r = calloc(1, 1);
    int size = 0;
    if (input_arguments[n] != 0)
    {
        r = realloc(r, size + 1 + strlen(input_arguments[n]));
        strcpy(r, input_arguments[n]);
    }
    return r;
}

char *expand_sharp()
{
    char *b = calloc(100, 1);
    int j = 0;
    while (intput_arguments[j] != '\0')
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
