#include <exec.h>
#include <parser.h>
#include <shell.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_prompt() {
  char *cwd = calloc(256, sizeof(char));
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
    if (!command) {
      fprintf(stderr, "parse error\n");
      continue;
    }
    exec_command(command);
  }
}
