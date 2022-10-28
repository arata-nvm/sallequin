#pragma once

#include <stddef.h>

typedef struct {
  char *file;
  char **args;
  size_t args_len;
} command_t;

typedef struct {
  command_t *command;

  struct command_list_t *next;
} command_list_t;

command_list_t *parse(char *s);
