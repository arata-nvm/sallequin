#include <exec_builtin.h>

#include <stdio.h>
#include <unistd.h>

int exec_builtin_cd(command_t *command) {
  if (command->args_len != 1)
    return 1;

  if (chdir(command->args[0]) == -1) {
    perror("chdir");
    return 1;
  }

  return 0;
}
