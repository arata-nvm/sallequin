#pragma once

typedef enum {
  TOKEN_WORD,
  TOKEN_NEWLINE,
  TOKEN_SEMI,
  TOKEN_EOF,
} tokentype_t;

typedef struct {
  tokentype_t type;
  char *literal;

  struct token_t *next;
} token_t;

token_t *tokenize(char *s);
