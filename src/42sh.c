#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc == 3 && !strcmp(argv[1], "-c"))
    {
        struct Token *tokens = lexer(argv[2]);
        printf("tokens:\n");
        print_token(tokens);
        printf("AST:\n");
        struct Node *root = parse(tokens);
        prettyprint(root, 0);

        return 0;
    }

    return 2;
}