#include "lexer/lexer.h"
#include "parser/parser_print.h"
#include "parser/parser.h"
#include "exec/exec.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *file_to_char(char *file)
{
    char *script = malloc(sizeof(char)), *line = NULL;
    size_t n = 1, len = 0;
    //ssize_t getline();
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("An error occured when openning %s\n", file);
    }
    while (getline(&line, &len, fp) != -1)
    {
        size_t i = 0;
        len = strlen(line);
        script = realloc(script, sizeof(char) * (n + len));
        while (i < len)
        {
            if (line[i] == '\n')
            {
                script[n - 1] = ';';
                i++;
                n++;
                continue;
            }
            script[n - 1] = line[i];
            i++;
            n++;
        }
    }
    if (line)
        free(line);
    script[n -  1] = '\0';
    return script;
}


int main(int argc, char **argv)
{
    /*printf("argv[0] : |%s\n", argv[0]);
    printf("argv[1] : |%s\n", argv[1]);
    printf("argv[2] : |%s\n", argv[2]);*/


    if (argc == 2)
    {
        char *script = file_to_char(argv[1]);
        if (script == NULL)
            return 1;
    }

    else if (argc >= 3 && !strcmp(argv[1], "-c"))
    {
        struct Token *tokens = lexer(argv[2]);
        /*printf("tokens:\n");
        print_token(tokens);*/
        struct Node *root = parse(tokens);

        // Print if asked in argument
        if (argc >= 4 && argv[3][0] == 't')
            print_token(tokens);
        if (argc >= 4 && (!strcmp(argv[3], "pp") || !strcmp(argv[3], "tpp")))
            prettyprint(root, stdout);
        else if (argc >= 4 && (!strcmp(argv[3], "sp") || !strcmp(argv[3], "tsp")))
            sexyprint(root);
        
        node_type(root);

        return 0;
    }

    else if ((strcmp(argv[1], "<") == 0))
    {
        printf ("%s", argv[1]);
        int dup_stdin = dup(STDIN_FILENO);
        int fd = open(argv[2], O_WRONLY | O_TRUNC, 0764);
        if (fd == -1)
            return 1;

        dup2(fd, STDIN_FILENO);
        char *script = file_to_char(argv[2]);
        if (script == NULL)
            return 1;
        //printf("from < : |%s\n", script);
        dup2(dup_stdin, STDIN_FILENO);
        close(dup_stdin);
    }
    return 0;
}