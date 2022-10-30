#pragma once

#include <stddef.h>
#include <stdbool.h>

struct command_t;

typedef enum {
  REDIRECT_INPUT,
  REDIRECT_OUTPUT,
  REDIRECT_OUTPUT_APPEND,
  REDIRECT_INOUT,
} redirect_type_t;

typedef struct redirects_t {
  redirect_type_t type;
  char *file;
  int fd;
} redirect_t;

typedef struct {
  char *file;
  char **args;
  size_t args_len;

  redirect_t *redirect;
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

typedef struct pipeline_command_t {
  struct command_t *command;
  bool negate_exit_code;

  struct pipeline_command_t *next;
} pipeline_command_t;

typedef enum {
  COMMAND_SIMPLE,
  COMMAND_LIST,
  COMMAND_SUBSHELL,
  COMMAND_PIPELINE,
} command_type_t;

typedef struct command_t {
  command_type_t type;

  union {
    simple_command_t *simple;
    list_command_t *list;
    subshell_command_t *subshell;
    pipeline_command_t *pipeline;
  } value;
} command_t;

command_t *parse(char *s);
