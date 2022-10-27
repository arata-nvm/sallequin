#pragma once

typedef enum {
  TOKEN_WORD,
  TOKEN_NEWLINE,
  TOKEN_EOF,
} tokentype_t;

typedef struct {
  tokentype_t type;
  char *literal;
} token_t;

char *dump_tokentype_t(tokentype_t tokentype);

void dump_token_t(token_t *token);

char *next_token(char *s, token_t *out_token);
