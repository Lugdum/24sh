#include "lexer/lexer.h"
#include "parser/parser.h"
#include "exec/exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    {
        struct Token *tokens = lexer(argv[2]);
        printf("tokens:\n");
        print_token(tokens);
        struct Node *root = parse(tokens);
//        prettyprint(root);
        printf("\nEXEX:\n");
        node_type(root);

        return 0;
    }

    return 2;
}
