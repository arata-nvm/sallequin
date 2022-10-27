#include <tokenizer.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *next_token(char *s, token_t *out_token) {
  while (*s == ' ')
    s++;

  if (*s == '\0') {
    out_token->type = TOKEN_EOF;
    out_token->literal = "\0";
    return s;
  }

  if (*s == '\n') {
    s++;
    out_token->type = TOKEN_NEWLINE;
    out_token->literal = "\n";
    return s;
  }

  char *literal_start = s;
  int literal_len = 0;
  while (!isspace(*s)) {
    literal_len++;
    s++;
  }

  char *literal = strndup(literal_start, literal_len);
  out_token->type = TOKEN_WORD;
  out_token->literal = literal;

  return s;
}
