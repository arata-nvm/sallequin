#include <parser.h>
#include <tokenizer.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool token_expect(char **s, tokentype_t expect_type, token_t *out_token) {
  *s = next_token(*s, out_token);

  if (out_token->type != expect_type) {
    fprintf(stderr, "parser: unexpected literal %s\n", out_token->literal);
    return false;
  }

  return true;
}

command_t *parse_command(char *command_str) {
  token_t *token = malloc(sizeof(token_t));

  if (!token_expect(&command_str, TOKEN_WORD, token))
    return NULL;
  char *file = token->literal;

  char **args = malloc(sizeof(char *) * 256);
  int args_len = 0;

  char *p = next_token(command_str, token);
  while (token->type == TOKEN_WORD) {
    args[args_len] = token->literal;
    args_len++;
    p = next_token(p, token);
  }

  command_t *command = malloc(sizeof(command_t));
  command->file = file;
  command->args = args;
  command->args_len = args_len;
  return command;
}
