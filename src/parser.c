#include <parser.h>
#include <tokenizer.h>

#include <stdlib.h>

token_t *parse_command(token_t *cur, command_t *out_command) {
  if (cur->type != TOKEN_WORD) return NULL;
  char *file = cur->literal;

  char **args = calloc(256, sizeof(char *));
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

token_t *parse_command_list(token_t *cur, command_list_t *out_command_list) {
  out_command_list->command = calloc(1, sizeof(command_t));
  cur = parse_command(cur, out_command_list->command);
  if (!cur) return NULL;

  for (;;) {
    switch (cur->type) {
      case TOKEN_SEMI:
        out_command_list->type = LIST_SEQUENTIAL;
        break;
      case TOKEN_AND:
        out_command_list->type = LIST_AND;
        break;
      case TOKEN_OR:
        out_command_list->type = LIST_OR;
        break;
      default:
        return cur;
    }
    int token_type = cur->type;
    cur = cur->next;

    if (cur->type != TOKEN_WORD) {
      if (token_type == TOKEN_SEMI) break;

      return NULL;
    }

    out_command_list->next = calloc(1, sizeof(command_list_t));
    out_command_list = out_command_list->next;

    out_command_list->command = calloc(1, sizeof(command_t));
    cur = parse_command(cur, out_command_list->command);
    if (!cur) return NULL;
  }

  return cur;
}

command_list_t *parse(char *s) {
  token_t *token = tokenize(s);

  command_list_t *command_list = calloc(1, sizeof(command_list_t));
  if (!parse_command_list(token, command_list)) {
    return NULL;
  }
  return command_list;
}