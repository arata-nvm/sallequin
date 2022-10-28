#include <parser.h>
#include <tokenizer.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

command_t *parse_command(char *command_str) {
  token_t *cur = tokenize(command_str);

  if (cur->type != TOKEN_WORD)
    return NULL;
  char *file = cur->literal;

  char **args = malloc(sizeof(char *) * 256);
  int args_len = 0;

  cur = cur->next;
  while (cur->type == TOKEN_WORD) {
    args[args_len] = cur->literal;
    args_len++;

    cur = cur->next;
  }

  command_t *command = malloc(sizeof(command_t));
  command->file = file;
  command->args = args;
  command->args_len = args_len;
  return command;
}
