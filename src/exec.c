#include <exec.h>
#include <exec_builtin.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void exec_external_command(command_t *command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  }

  if (pid == 0) {
    size_t argv_len = command->args_len + 2;
    char **argv = malloc(sizeof(char *) * argv_len);
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
    waitpid(pid, NULL, 0);
  }
}

void exec_command(command_t *command) {
  if (!strcmp(command->file, "cd")) {
    exec_builtin_cd(command);
  } else {
    exec_external_command(command);
  }
}
