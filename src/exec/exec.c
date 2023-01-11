#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "exec.h"

int echo(struct Node *ast, int fd) {
  int flag_n = 0;
  int flag_e = 0;
  int flag_E = 0;
  int word = 0;
  int son = 0;
  while (!word && son < ast->nb_children) {
    son++;
    char *flag = ast->children[son]->value;
    if (!strcmp(flag, "-e")) {
      flag_e = 1;
    } else if (!strcmp(flag, "-E")) {
      flag_E = 1;
    } else if (!strcmp(flag, "-n")) {
      flag_n = 1;
    } else if (!strcmp(flag, "-en")) {
      flag_e = 1;
      flag_n = 1;
    } else if (!strcmp(flag, "-ne")) {
      flag_e = 1;
      flag_n = 1;
    } else if (!strcmp(flag, "-En")) {
      flag_E = 1;
      flag_n = 1;
    } else if (!strcmp(flag, "-Ee")) {
      flag_e = 1;
      flag_E = 1;
    } else
      word = 1;
  }

  for (int i = son; i < ast->nb_children; i++) {
    if (flag_e) {
      // Remplace les séquences d'échappement précédées de \ par leurs
      // équivalents ASCII
      for (int j = 0; ast->children[i]->value[j] != '\0'; j++) {
        if (ast->children[i]->value[j] == '\\' && flag_E) {
          switch (ast->children[i]->value[j + 1]) {
          case 'a':
            putchar('\a');
            break;
          case 'b':
            putchar('\b');
            break;
          case 'f':
            putchar('\f');
            break;
          case 'n':
            putchar('\n');
            break;
          case 'r':
            putchar('\r');
            break;
          case 't':
            putchar('\t');
            break;
          case 'v':
            putchar('\v');
            break;
          default:
            putchar(ast->children[i]->value[j]);
          }
          j++;
        } else {
          putchar(ast->children[i]->value[j]);
        }
      }
    } else {
      printf("%s", ast->children[i]->value);
    }

    // Ajoute un espace entre chaque argument, sauf pour le dernier
    if (i < ast->nb_children - son) {
      putchar(' ');
    }
  }

  if (!flag_n) {
    putchar('\n');
  }
  return 1;
}
int exec_command(struct Node *ast) {
  if (!strcmp(ast->children[0]->value, "echo")) {
    return echo(ast, 1);
  }
  if (!strcmp(ast->children[0]->value, "true")) {
    return 1;
  }
  if (!strcmp(ast->children[0]->value, "false")) {
    return 0;
  }
  return 1;
}

int process_if(struct Node *ast) {
  if (node_type(ast->children[0]))
    return node_type(ast->children[1]);
  else
    return node_type(ast->children[2]);
}

int process_cmd(struct Node *ast) { 
    if (ast->children[0]->type == AST_SIMPLE_COMMAND)
        return node_type(ast->children[0]);
    else
        return exec_command(ast); 
}

int process_list(struct Node *ast) {
  int r = 0;
  for (int i = 0; i < ast->nb_children; i++)
    r = node_type(ast->children[i]);
  return r;
}

int process_pipe(struct Node *ast) {
  // int fd = -1;
  node_type(ast->children[0]);
  return node_type(ast->children[1]);
}

int node_type(struct Node *ast) {
  switch (ast->type) {
  case AST_INPUT:
    return process_cmd(ast);
  case AST_LIST:
    return process_list(ast);
  case AST_AND_OR:
    // TODO
    break;
  case AST_PIPELINE:
    // TODO
    break;
  case AST_COMMAND:
    return process_cmd(ast);
  case AST_SIMPLE_COMMAND:
    return exec_command(ast);
  case AST_ELEMENT:
    // TODO
    break;
  case AST_CMD:
    // TODO
    break;
  case AST_IF:
    return process_if(ast);
  default:
    printf("Unknown Node type\n");
    return 2;
  }
  return 0;
}