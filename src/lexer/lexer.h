#ifndef LEXER_H
#define LEXER_H

enum TokenType
{
    IF,
    THEN,
    ELIF,
    ELSE,
    FI,
    SC,
    EOF,
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

struct Token *lex(char *arg);

void print_token(struct Token *token);

#endif /* !LEXER_H */