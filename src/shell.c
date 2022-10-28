#include <exec.h>
#include <parser.h>
#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_prompt() {
  char *cwd = malloc(sizeof(char) * 256);
  getcwd(cwd, 256);
  printf("%s$ ", cwd);
}

void interactive_shell() {
  char command_buf[256];

  for (;;) {
    print_prompt();
    if (!fgets(command_buf, 256, stdin)) {
      exit(0);
    }

    command_t *command = parse(command_buf);
    if (!command)
      continue;
    exec_command(command);
    free(command->args);
    free(command);
  }
}
