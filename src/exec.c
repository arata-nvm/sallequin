#include <exec.h>
#include <exec_builtin.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int exec_external_command(command_t *command) {
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
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    return WEXITSTATUS(stat);
  }
}

int exec_command(command_t *command) {
  if (!strcmp(command->file, "cd")) {
    return exec_builtin_cd(command);
  } else {
    return exec_external_command(command);
  }
}

int exec_command_list(command_list_t *command_list) {
  int last_exit_code = 0;
  
  command_list_t *cur = command_list;
  while (cur) {
    last_exit_code = exec_command(cur->command);
    cur = cur->next;
  }

  return last_exit_code;
}
