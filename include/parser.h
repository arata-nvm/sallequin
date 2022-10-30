#pragma once

#include <stddef.h>
#include <stdbool.h>

struct command_t;

typedef struct {
  char *file;
  char **args;
  size_t args_len;
} simple_command_t;

typedef enum {
  LIST_SEQUENTIAL,
  LIST_AND,
  LIST_OR,
} list_command_type_t;

typedef struct list_command_t {
  list_command_type_t type;
  struct command_t *command;

  struct list_command_t *next;
} list_command_t;

typedef struct {
  struct command_t *command;
} subshell_command_t;

typedef enum {
  COMMAND_SIMPLE,
  COMMAND_LIST,
  COMMAND_SUBSHELL,
} command_type_t;

typedef struct command_t {
  command_type_t type;

  union {
    simple_command_t *simple;
    list_command_t *list;
    subshell_command_t *subshell;
  } value;
} command_t;

typedef struct pipeline_command_t {
  struct command_t *command;
  bool negate_exit_code;

  struct pipeline_command_t *next;
} pipeline_command_t;

typedef pipeline_command_t complete_command_t;

complete_command_t *parse(char *s);
