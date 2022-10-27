#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char *file;
} command_t;

command_t *parse_command(char *command_str) {
  int command_len = strlen(command_str);
  command_str[command_len - 1] = '\0';

  command_t *command = malloc(sizeof(command_t));
  command->file = command_str;
  return command;
}

void exec_command(command_t *command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
  }

  if (pid == 0) {
    char *command_args[] = {command->file, NULL};
    if (execvp(command->file, command_args) == -1) {
      perror("execvp");
      exit(1);
    }
  } else {
    waitpid(pid, NULL, 0);
  }
}

int main() {
  char command_buf[256];

  for (;;) {
    printf("$ ");
    if (!fgets(command_buf, 256, stdin)) {
      exit(0);
    }

    command_t *command = parse_command(command_buf);
    exec_command(command);
    free(command);
  }

  return 0;
}
