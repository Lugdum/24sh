

struct variable_list
{
    int size;
    struct variable *list;
};

struct variable
{
    char *name;
    char *value;
};

char *find_value(struct variable_list *list, char *name);
void insert_value(struct variable_list *list, char *name, char *value);
void modify_value(struct variable_list *list, char *name, char *value);
void free_list(struct variable_list *list);