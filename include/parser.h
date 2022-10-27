#pragma once

#include <stddef.h>

typedef struct {
  char *file;
  char **args;
  size_t args_len;
} command_t;

command_t *parse_command(char *command_str);
