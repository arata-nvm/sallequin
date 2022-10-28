#pragma once

#include <parser.h>

int exec_external_command(command_t *command);

int exec_command(command_t *command);

int exec_command_list(command_list_t *command_list);
