#include <parser.h>
#include <tokenizer.h>

#include <stdlib.h>

token_t *parse_command(token_t *cur, command_t *command);

token_t *parse_simple_command(token_t *cur, simple_command_t *out_simple) {
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

  out_simple->file = file;
  out_simple->args = args;
  out_simple->args_len = args_len;
  return cur;
}

token_t *parse_subshell_command(token_t *cur, subshell_command_t *out_subshell) {
  if (cur->type != TOKEN_PAREN_OPEN) return NULL;
  cur = cur->next;

  out_subshell->command = calloc(1, sizeof(command_t));
  cur = parse_command(cur, out_subshell->command);

  if (cur->type != TOKEN_PAREN_CLOSE) return NULL;
  cur = cur->next;

  return cur;
}

token_t *parse_list_command_inner(token_t *cur, command_t *out_command) {
  switch (cur->type) {
    case TOKEN_PAREN_OPEN:
      out_command->type = COMMAND_SUBSHELL;
      out_command->value.subshell = calloc(1, sizeof(subshell_command_t));
      return parse_subshell_command(cur, out_command->value.subshell);
    default:
      out_command->type = COMMAND_SIMPLE;
      out_command->value.simple = calloc(1, sizeof(simple_command_t));
      return parse_simple_command(cur, out_command->value.simple);
  }
}

token_t *parse_list_command(token_t *cur, list_command_t *out_list) {
  out_list->command = calloc(1, sizeof(command_t));
  cur = parse_list_command_inner(cur, out_list->command);
  if (!cur) return NULL;

  for (;;) {
    switch (cur->type) {
      case TOKEN_SEMI:
        out_list->type = LIST_SEQUENTIAL;
        break;
      case TOKEN_AND:
        out_list->type = LIST_AND;
        break;
      case TOKEN_OR:
        out_list->type = LIST_OR;
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

    out_list->next = calloc(1, sizeof(list_command_t));
    out_list = out_list->next;

    out_list->command = calloc(1, sizeof(simple_command_t));
    cur = parse_list_command_inner(cur, out_list->command);
    if (!cur) return NULL;
  }

  return cur;
}

token_t *parse_command(token_t *cur, command_t *out_command) {
  switch (cur->type) {
    case TOKEN_PAREN_OPEN:
      out_command->type = COMMAND_SUBSHELL;
      out_command->value.subshell = calloc(1, sizeof(subshell_command_t));
      return parse_subshell_command(cur, out_command->value.subshell);
    default:
      out_command->type = COMMAND_LIST;
      out_command->value.list = calloc(1, sizeof(list_command_t));
      return parse_list_command(cur, out_command->value.list);
  }
}

command_t *parse(char *s) {
  token_t *token = tokenize(s);

  command_t *command = calloc(1, sizeof(command_t));
  command->type = COMMAND_LIST;
  command->value.list = calloc(1, sizeof(list_command_t));
  if (!parse_list_command(token, command->value.list)) {
    return NULL;
  }
  return command;
}