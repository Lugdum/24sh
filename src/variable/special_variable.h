#ifndef SPECIAL_VARIABLE_H
#define SPECIAL_VARIABLE_H


char **expand_at();
char *expand_star();
char *expand_question_mark();
char *expand_dollar();
char *expand_n(int n);
char *expand_sharp();
char *expand_uid();
char *expand_uid();
char *expand_pwd();
char *expand_random();

#endif /* ! SPECIAL_VARIABLE_H */
