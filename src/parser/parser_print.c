#include "parser_print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

// Print AST in dot
void prettyprint(struct Node *ast, FILE *f)
{
    printf("AST:\n");
    fprintf(f, "digraph ast {\n");
    fprintf(f, "node [shape=box];\n");
    print_node(ast, 0, f);
    fprintf(f, "}\n");
}

void sexyprint(struct Node *ast)
{
    FILE *f = fopen("./graph", "w");
    fprintf(f, "digraph ast {\n");
    fprintf(f, "node [shape=box];\n");
    print_node(ast, 0, f);
    fprintf(f, "}\n");
    fclose(f);
}

void print_node(struct Node *node, int parent, FILE *f)
{
    static int node_count = 1;
    int current_node = node_count++;
    switch (node->type)
    {
    case AST_INPUT:
        fprintf(f, "node%d [label=\"AST_INPUT\"];\n", current_node);
        break;
    case AST_OR:
        fprintf(f, "node%d [label=\"OR\"];\n", current_node);
        break;
    case AST_AND:
        fprintf(f, "node%d [label=\"AND\"];\n", current_node);
        break;
    case AST_LIST:
        fprintf(f, "node%d [label=\"LIST\"];\n", current_node);
        break;
    case AST_PIPELINE:
        fprintf(f, "node%d [label=\"PIPELINE\"];\n", current_node);
        break;
    case AST_COMMAND:
        fprintf(f, "node%d [label=\"COMMAND\"];\n", current_node);
        break;
    case AST_SIMPLE_COMMAND:
        fprintf(f, "node%d [label=\"SIMPLE_COMMAND\"];\n", current_node);
        break;
    case AST_ELEMENT:
        fprintf(f, "node%d [label=\"%s\"];\n", current_node, node->value);
        break;
    case AST_CMD:
        fprintf(f, "node%d [label=\"CMD\"];\n", current_node);
        break;
    case AST_IF:
        fprintf(f, "node%d [label=\"IF\"];\n", current_node);
        break;
    case AST_FOR:
        fprintf(f, "node%d [label=\"FOR\"];\n", current_node);
        break;
    case AST_WHILE:
        fprintf(f, "node%d [label=\"WHILE\"];\n", current_node);
        break;
    case AST_UNTIL:
        fprintf(f, "node%d [label=\"UNTIL\"];\n", current_node);
        break;
    default:
        fprintf(f, "node%d [label=\"UNKNOWN\"];\n", current_node);
        break;
    }
    if (parent != 0)
    {
        fprintf(f, "node%d -> node%d;\n", parent, current_node);
    }
    for (int i = 0; i < node->nb_children; i++)
    {
        print_node(node->children[i], current_node, f);
    }
}