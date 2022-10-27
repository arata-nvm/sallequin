#include <exec_builtin.h>

#include <stdio.h>
#include <unistd.h>

void exec_builtin_cd(command_t *command) {
  if (command->args_len != 1)
    return;

  if (chdir(command->args[0]) == -1) {
    perror("chdir");
  }
}
