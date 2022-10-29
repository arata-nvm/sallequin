#pragma once

#include <parser.h>

int exec_external_command(simple_command_t *command);

int exec_simple_command(simple_command_t *simple_command);

int exec_list_command(list_command_t *list_command);

int exec_command(command_t *command);

int exec(complete_command_t *command);
