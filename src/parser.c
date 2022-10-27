#include <parser.h>

#include <stdlib.h>
#include <string.h>

command_t *parse_command(char *command_str) {
  int command_len = strlen(command_str);
  command_str[command_len - 1] = ' ';

  char **args = malloc(sizeof(char *) * 256);
  char *file = strtok(command_str, " ");

  int i = 0;
  char *p = strtok(NULL, " ");
  while (p) {
    args[i++] = p;
    p = strtok(NULL, " ");
  }

  command_t *command = malloc(sizeof(command_t));
  command->file = file;
  command->args = args;
  command->args_len = i;

  return command;
}
