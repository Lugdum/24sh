#ifndef SPECIAL_VARIABLE_H
#define SPECIAL_VARIABLE_H

char **expand_at(void);
char *expand_star(void);
char *expand_question_mark(void);
char *expand_dollar(void);
char *expand_n(int n);
char *expand_sharp(void);
char *expand_uid(void);
char *expand_uid(void);
char *expand_pwd(void);
char *expand_random(void);

#endif /* ! SPECIAL_VARIABLE_H */
