#include "lexer/lexer.h"
#include "parser/parser.h"
#include "exec/exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc >= 3 && !strcmp(argv[1], "-c"))
    {
        struct Token *tokens = lexer(argv[2]);
        printf("tokens:\n");
        print_token(tokens);
        struct Node *root = parse(tokens);

        // Print if asked in argument
        if (argc >= 4 && !strcmp(argv[3], "pp"))
            prettyprint(root, stdout);
        if (argc >= 4 && !strcmp(argv[3], "sp"))
            sexyprint(root);
        
        // Print result
        printf("\nEXEX:\n");
        node_type(root);

        return 0;
    }

    return 2;
}
