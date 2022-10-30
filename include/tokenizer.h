#pragma once

typedef enum {
  TOKEN_WORD,
  TOKEN_NEWLINE,
  TOKEN_SEMI,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_PAREN_OPEN,
  TOKEN_PAREN_CLOSE,
  TOKEN_BANG,
  TOKEN_PIPE,
  TOKEN_LESS,
  TOKEN_EOF,
} tokentype_t;

typedef struct token_t {
  tokentype_t type;
  char *literal;

  struct token_t *next;
} token_t;

token_t *tokenize(char *s);
