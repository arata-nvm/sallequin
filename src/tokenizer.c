#include <tokenizer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

bool is_number(char *s) {
  while (*s) {
    if (!isnumber(*s)) return false;
    s++;
  }
  return true;
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

void set_token(token_t *token, tokentype_t type, char *literal) {
  token->type = type;
  token->literal = literal;
}

char *next_token(char *s, token_t *out_token) {
  while (*s == ' ')
    s++;

  int len;
  switch (*s) {
    case '\0':
      set_token(out_token, TOKEN_EOF, "\0");
      return s;
    case '\n':
      set_token(out_token, TOKEN_NEWLINE, "\n");
      break;
    case ';':
      set_token(out_token, TOKEN_SEMI, ";");
      break;
    case '&':
      if (*(s + 1) == '&') {
        s += 2;
        set_token(out_token, TOKEN_AND, "&&");
        return s;
      }

      // TODO
      set_token(out_token, TOKEN_WORD, "&");
      break;
    case '|':
      if (*(s + 1) == '|') {
        s += 2;
        set_token(out_token, TOKEN_OR, "||");
        return s;
      }
      set_token(out_token, TOKEN_PIPE, "|");
      break;
    case '(':
      set_token(out_token, TOKEN_PAREN_OPEN, "(");
      break;
    case ')':
      set_token(out_token, TOKEN_PAREN_CLOSE, ")");
      break;
    case '!':
      set_token(out_token, TOKEN_BANG, "!");
      break;
    case '<':
      if (*(s + 1) == '>') {
        s += 2;
        set_token(out_token, TOKEN_LESSGREAT, "<>");
        return s;
      } else if (*(s + 1) == '&') {
        s += 2;
        set_token(out_token, TOKEN_LESSAND, "<&");
        return s;
      }
      set_token(out_token, TOKEN_LESS, "<");
      break;
    case '>':
      if (*(s + 1) == '>') {
        s += 2;
        set_token(out_token, TOKEN_DGREAT, ">>");
        return s;
      } else if (*(s + 1) == '&') {
        s += 2;
        set_token(out_token, TOKEN_GREATAND, ">&");
        return s;
      }
      set_token(out_token, TOKEN_GREAT, ">");
      break;
    default:
      len = consume_word(s, out_token);
      s += len;

      if ((*s == '<' || *s == '>') && is_number(out_token->literal)) {
        out_token->type = TOKEN_IONUMBER;
      }
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
