#include <exec_builtin.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int exec_builtin_cd(simple_command_t *command) {
  if (command->args_len != 1)
    return 1;

  if (chdir(command->args[0]) == -1) {
    perror("chdir");
    return 1;
  }

  return 0;
}

int exec_builtin_exec(simple_command_t *command) {
  if (command->args_len == 0)
    return 1;

  size_t argv_len = command->args_len + 1;
  char **argv = calloc(argv_len, sizeof(char *));
  for (size_t i = 0; i < command->args_len; i++) {
    argv[i] = command->args[i];
  }
  argv[argv_len - 1] = NULL;

  if (execvp(command->args[0], argv) == -1) {
    perror("execvp");
    return 1;
  }

  __builtin_unreachable();
}

void exec_builtin_exit(simple_command_t *command) {
  if (command->args_len == 1) {
    char *endptr;
    int exit_code = (int) strtol(command->args[0], &endptr, 10);
    if (endptr == command->args[0]) {
      fprintf(stderr, "exit: invalid integer");
      exit(1);
    }
    exit(exit_code);
  }
  exit(0);
}