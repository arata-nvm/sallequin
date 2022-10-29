#include <exec.h>
#include <exec_builtin.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int exec_external_command(simple_command_t *command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  }

  if (pid == 0) {
    size_t argv_len = command->args_len + 2;
    char **argv = calloc(argv_len, sizeof(char *));
    argv[0] = command->file;
    argv[argv_len - 1] = NULL;
    for (size_t i = 0; i < command->args_len; i++) {
      argv[i + 1] = command->args[i];
    }

    if (execvp(command->file, argv) == -1) {
      perror("execvp");
      exit(1);
    }

    __builtin_unreachable();
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    return WEXITSTATUS(stat);
  }
}

int exec_simple_command(simple_command_t *simple_command) {
  int exit_code;
  if (!strcmp(simple_command->file, "cd")) {
    exit_code = exec_builtin_cd(simple_command);
  } else if (!strcmp(simple_command->file, "exec")) {
    exit_code = exec_builtin_exec(simple_command);
  } else if (!strcmp(simple_command->file, "exit")) {
    exec_builtin_exit(simple_command);
  } else {
    exit_code = exec_external_command(simple_command);
  }

  if (simple_command->negate_exit_code) {
    if (exit_code == 0) {
      exit_code = 1;
    } else {
      exit_code = 0;
    }
  }

  return exit_code;
}

int exec_list_command(list_command_t *list_command) {
  int last_exit_code = 0;

  list_command_t *cur = list_command;
  while (cur) {
    last_exit_code = exec_command(cur->command);

    switch (cur->type) {
      case LIST_SEQUENTIAL:
        break;
      case LIST_AND:
        if (last_exit_code != 0) return last_exit_code;
        break;
      case LIST_OR:
        if (last_exit_code == 0) return last_exit_code;
        break;
    }

    cur = cur->next;
  }

  return last_exit_code;
}

int exec_subshell_command(subshell_command_t *subshell_command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  }

  if (pid == 0) {
    exec_command(subshell_command->command);
    exit(0);
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    return WEXITSTATUS(stat);
  }
}

int exec_command(command_t *command) {
  switch (command->type) {
    case COMMAND_SIMPLE:
      return exec_simple_command(command->value.simple);
    case COMMAND_LIST:
      return exec_list_command(command->value.list);
    case COMMAND_SUBSHELL:
      return exec_subshell_command(command->value.subshell);
  }
}

int exec(complete_command_t *command) {
  return exec_command(command);
}
