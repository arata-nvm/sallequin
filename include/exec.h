#pragma once

#include <parser.h>

int exec_external_command(simple_command_t *command);

int exec_simple_command(simple_command_t *simple_command);

int exec_command_list(command_list_t *command_list);

int exec_command(command_t *command);
