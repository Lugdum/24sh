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
    SC,
    SQ,
    PIPE,
    WORD
};

struct Token
{
    enum TokenType type;
    char *value;
    int single_quoted;
    struct Token *next;
};

struct Token *lexer(char *arg);
void free_lexer(struct Token *token);

void print_token(struct Token *token);

#endif /* !LEXER_H */
