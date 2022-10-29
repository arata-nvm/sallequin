#include <parser.h>

int exec_builtin_cd(simple_command_t *command);

int exec_builtin_exec(simple_command_t *command);

__attribute__((noreturn))
void exec_builtin_exit(simple_command_t *command);
