#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char *file;
  char **args;
  size_t args_len;
} command_t;

command_t *parse_command(char *command_str) {
  int command_len = strlen(command_str);
  command_str[command_len - 1] = ' ';

  char **args = malloc(sizeof(char *) * 256);
  char *file = strtok(command_str, " ");

  int i = 0;
  char *p = file;
  while (p) {
    p = strtok(NULL, " ");
    args[i] = p;
    i++;
  }

  command_t *command = malloc(sizeof(command_t));
  command->file = command_str;
  command->args = args;
  command->args_len = i;

  return command;
}

void exec_command(command_t *command) {
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

int main() {
  char command_buf[256];

  for (;;) {
    printf("$ ");
    if (!fgets(command_buf, 256, stdin)) {
      exit(0);
    }

    command_t *command = parse_command(command_buf);
    exec_command(command);
    free(command->args);
    free(command);
  }

  return 0;
}
