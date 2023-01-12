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
    IN,
    DO,
    DONE,
    SC,
    EF,
    NL,
    SQ,
    AND,
    OR,
    PIPE,
    WORD
};

struct Token
{
    enum TokenType type;
    char *value;
    struct Token *next;
};

struct Token *lexer(char *arg);
void free_lexer(struct Token *token);

void print_token(struct Token *token);

#endif /* !LEXER_H */