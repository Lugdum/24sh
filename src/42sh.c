#define _POSIX_C_SOURCE 200809L

#include "lexer/lexer.h"
#include "parser/parser_print.h"
#include "parser/parser.h"
#include "parser/function.h"
#include "parser/ast.h"
#include "exec/exec.h"
#include "variable/variable.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern struct Function *functions;

int main(int argc, char **argv)
{
    int res = 1;
    struct Token *tokens;
    char **input_args;
    
    //load token from diff sources
    // from stdin
    if (argc == 1)
    {
        char *buffer = calloc(20000, 1);
        ssize_t rr = read(STDIN_FILENO, buffer, 20000);
        if (rr > 0)
        {
            tokens = lexer(buffer);
            free(buffer);
        }
        else
        {
            free(buffer);
            return 0;
        }
        input_args = NULL;
    }
    //from file
    else if (argc >= 2 && strcmp(argv[1], "-c"))
    {
        char *script = file_to_char(argv[1]);
        if (script == NULL)
        {
            free(script);
            return 1;
        }
        tokens = lexer(script);
        //printf("%s\n", script);
        free(script);
        //argument given to the shell script
        input_args = argv + 2;
    }
    //script given directly as parameter
    else if (argc >= 3 && !strcmp(argv[1], "-c"))
    {
        tokens = lexer(argv[2]);
        input_args = argv + 3;
    }
    else
    {
        fprintf(stderr, "Wrong arguments\n");
        return res;
    }
    //print tokens
    if (argc >= 3 && argv[argc-1][0] == 't')
        print_token(tokens);

    // Parse tokens
    struct Node *ast = NULL;
    res = parse(tokens, &ast);
    if (res)
    {
        free_lexer(tokens);
        free_functions(functions);
        return res;
    }

    // Print if asked in argument
    if (argc >= 3 && (!strcmp(argv[argc-1], "pp") || !strcmp(argv[argc-1], "tpp") || !strcmp(argv[argc-1], "tppf")))
        prettyprint(ast, stdout);
    else if (argc >= 3 && (!strcmp(argv[argc-1], "sp") || !strcmp(argv[argc-1], "tsp") || !strcmp(argv[argc-1], "tspf")))
        sexyprint(ast);
    if (argc >= 3 && (argv[argc-1][strlen(argv[argc-1])-1] == 'f'))
        printFunction();
    
    // exec script if no error
    if (!res && ast)
        res = main_exec(ast, input_args);

    free_lexer(tokens);
    free_ast(ast);
    free_functions(functions);
    return res;
}
