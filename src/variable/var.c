#include "variable.h"
#include "var.h"

#include <stdlib.h>


void define_var_list(void)
{
    var_list = calloc(1, sizeof(struct variable_list));
    var_list->size = 0;
    var_list->list = NULL;
}
