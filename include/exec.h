#pragma once

#include <parser.h>

void exec_external_command(command_t *command);

void exec_command(command_t *command);

void exec_command_list(command_list_t *command_list);
