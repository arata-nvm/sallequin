#include <parser.h>
#include <tokenizer.h>

#include <stdlib.h>

token_t *parse_list_command(token_t *cur, command_t *out_command);

token_t *parse_redirect(token_t *cur, redirect_t *out_redirect) {
  out_redirect->fd = -1;
  if (cur->type == TOKEN_IONUMBER) {
    out_redirect->fd = (int) strtol(cur->literal, NULL, 10);
    cur = cur->next;
  }

  switch (cur->type) {
    case TOKEN_LESS:
      out_redirect->type = REDIRECT_INPUT;
      break;
    case TOKEN_GREAT:
      out_redirect->type = REDIRECT_OUTPUT;
      break;
    case TOKEN_DGREAT:
      out_redirect->type = REDIRECT_OUTPUT_APPEND;
      break;
    case TOKEN_LESSGREAT:
      out_redirect->type = REDIRECT_INOUT;
      break;
    default:
      return NULL;
  }
  cur = cur->next;

  if (cur->type != TOKEN_WORD) return NULL;
  out_redirect->file = cur->literal;
  cur = cur->next;

  return cur;
}

token_t *parse_simple_command(token_t *cur, command_t *out_command) {
  out_command->type = COMMAND_SIMPLE;
  out_command->value.simple = calloc(1, sizeof(simple_command_t));
  simple_command_t *out_simple = out_command->value.simple;

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

  token_t *prev_cur = cur;
  redirect_t *redirect = calloc(1, sizeof(redirect_t));
  cur = parse_redirect(cur, redirect);
  if (!cur) return prev_cur;
  out_simple->redirect = redirect;

  return cur;
}

token_t *parse_subshell_command(token_t *cur, command_t *out_command) {
  out_command->type = COMMAND_SUBSHELL;
  out_command->value.subshell = calloc(1, sizeof(subshell_command_t));
  subshell_command_t *out_subshell = out_command->value.subshell;

  if (cur->type != TOKEN_PAREN_OPEN) return NULL;
  cur = cur->next;

  out_subshell->command = calloc(1, sizeof(command_t));
  cur = parse_list_command(cur, out_subshell->command);
  if (!cur) return NULL;

  if (cur->type != TOKEN_PAREN_CLOSE) return NULL;
  cur = cur->next;

  return cur;
}

token_t *parse_command(token_t *cur, command_t *command) {
  switch (cur->type) {
    case TOKEN_PAREN_OPEN:
      return parse_subshell_command(cur, command);
    default:
      return parse_simple_command(cur, command);
  }
}

token_t *parse_pipeline_command(token_t *cur, command_t *out_command) {
  out_command->type = COMMAND_PIPELINE;
  out_command->value.pipeline = calloc(1, sizeof(pipeline_command_t));
  pipeline_command_t *out_pipeline = out_command->value.pipeline;

  if (cur->type == TOKEN_BANG) {
    out_pipeline->negate_exit_code = true;
    cur = cur->next;
  }

  out_pipeline->command = calloc(1, sizeof(command_t));
  cur = parse_command(cur, out_pipeline->command);
  if (!cur) return NULL;

  while (cur->type == TOKEN_PIPE) {
    cur = cur->next;

    out_pipeline->next = calloc(1, sizeof(pipeline_command_t));
    out_pipeline = out_pipeline->next;

    out_pipeline->command = calloc(1, sizeof(command_t));
    cur = parse_command(cur, out_pipeline->command);
    if (!cur) return NULL;
  }

  return cur;
}

token_t *parse_list_command(token_t *cur, command_t *out_command) {
  out_command->type = COMMAND_LIST;
  out_command->value.list = calloc(1, sizeof(list_command_t));
  list_command_t *out_list = out_command->value.list;

  out_list->command = calloc(1, sizeof(command_t));
  cur = parse_pipeline_command(cur, out_list->command);
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
    cur = cur->next;

    token_t *prev_cur = cur;
    command_t *command = calloc(1, sizeof(command_t));
    cur = parse_pipeline_command(cur, command);
    if (!cur) return prev_cur;

    out_list->next = calloc(1, sizeof(list_command_t));
    out_list = out_list->next;
    out_list->command = command;
  }
}

command_t *parse(char *s) {
  token_t *token = tokenize(s);

  command_t *command = calloc(1, sizeof(command_t));
  if (!parse_list_command(token, command)) {
    return NULL;
  }
  return command;
}
