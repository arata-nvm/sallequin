#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  char command_buf[256];
  int command_len;

  for (;;) {
    printf("$ ");
    if (!fgets(command_buf, 256, stdin)) {
      exit(0);
    }

    command_len = strlen(command_buf);
    command_buf[command_len - 1] = '\0';

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
    }

    if (pid == 0) {
      char *command_args[] = {command_buf, NULL};
      if (execvp(command_buf, command_args) == -1) {
        perror("execvp");
        exit(1);
      }
    } else {
      waitpid(pid, NULL, 0);
    }
  }

  return 0;
}
