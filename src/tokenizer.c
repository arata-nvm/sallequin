#include <tokenizer.h>

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool is_word_char(char c) {
  return (
      c != '|' &&
      c != '&' &&
      c != ';' &&
      c != '<' &&
      c != '>' &&
      c != '(' &&
      c != ')' &&
      c != '$' &&
      c != '`' &&
      c != '\\' &&
      c != '"' &&
      c != '\'' &&
      c != ' ' &&
      c != '\t' &&
      c != '\n'
  );
}

int consume_word(char *s, token_t *out_token) {
  char *literal_start = s;
  int literal_len = 0;
  while (is_word_char(*s)) {
    literal_len++;
    s++;
  }

  char *literal = strndup(literal_start, literal_len);
  out_token->type = TOKEN_WORD;
  out_token->literal = literal;

  return literal_len;
}

char *next_token(char *s, token_t *out_token) {
  while (*s == ' ')
    s++;

  int len;
  switch (*s) {
    case '\0':
      out_token->type = TOKEN_EOF;
      out_token->literal = "\0";
      return s;
    case '\n':
      out_token->type = TOKEN_NEWLINE;
      out_token->literal = "\n";
      break;
    case ';':
      out_token->type = TOKEN_SEMI;
      out_token->literal = ";";
      break;
    default:
      len = consume_word(s, out_token);
      s += len;
      return s;
  }

  s++;

  return s;
}

token_t *tokenize(char *s) {
  token_t *first_token = calloc(1, sizeof(token_t));
  token_t *cur = first_token;

  s = next_token(s, cur);
  while (cur->type != TOKEN_EOF) {
    cur->next = calloc(1, sizeof(token_t));
    cur = cur->next;

    s = next_token(s, cur);
  }

  return first_token;
}
