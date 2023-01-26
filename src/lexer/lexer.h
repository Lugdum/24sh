#ifndef LEXER_H
#define LEXER_H

enum TokenType
{
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,
    FOR,
    WHILE,
    UNTIL,
    IN,
    DO,
    DONE,
    AND,
    OR,
    EF,
    NL,
    PIPE,
    B_OP,
    B_CL,
    LP,
    RP,
    SC,
    EM,
    SQ,
    WORD,
    LS,
    PL,
    PR,
    CHEVREL,
    CHEVRED,
    CASE,
    ESAC,
    VIRG,
};

struct Token
{
    enum TokenType type;
    char *value;
    struct Token *next;
};

struct Token *lexer(char *arg);
void free_lexer(struct Token *token);
char *file_to_char(char *file);

void print_token(struct Token *token);

#endif /* !LEXER_H */
