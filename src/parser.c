#include <parser.h>
#include <tokenizer.h>

#include <stdlib.h>

token_t *parse_command(token_t *cur, command_t *out_command) {
  if (cur->type != TOKEN_WORD) return NULL;
  char *file = cur->literal;

  char **args = malloc(sizeof(char *) * 256);
  int args_len = 0;

  cur = cur->next;
  while (cur->type == TOKEN_WORD) {
    args[args_len] = cur->literal;
    args_len++;

    cur = cur->next;
  }

  out_command->file = file;
  out_command->args = args;
  out_command->args_len = args_len;
  return cur;
}

command_t *parse(char *s) {
  token_t *token = tokenize(s);

  command_t *command = malloc(sizeof(command_t));
  if (!parse_command(token, command)) {
    return NULL;
  }
  return command;
}