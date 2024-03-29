#include <exec.h>
#include <exec_builtin.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>

int exec_redirect(redirect_t *redirect) {
  int fd_from;
  int fd_to = redirect->fd;
  switch (redirect->type) {
    case REDIRECT_INPUT:
      fd_from = open(redirect->file, O_RDONLY);
      if (fd_to == -1) fd_to = 0;
      break;
    case REDIRECT_OUTPUT:
      fd_from = open(redirect->file, O_CREAT | O_TRUNC | O_WRONLY);
      if (fd_to == -1) fd_to = 1;
      break;
    case REDIRECT_OUTPUT_APPEND:
      fd_from = open(redirect->file, O_CREAT | O_APPEND | O_WRONLY);
      if (fd_to == -1) fd_to = 1;
      break;
    case REDIRECT_INOUT:
      fd_from = open(redirect->file, O_CREAT | O_WRONLY);
      if (fd_to == -1) fd_to = 0;
      break;
    case REDIRECT_INPUT_DUP:
      fd_from = redirect->fd_dup;
      if (fd_to == -1) fd_to = 0;
      break;
    case REDIRECT_OUTPUT_DUP:
      fd_from = redirect->fd_dup;
      if (fd_to == -1) fd_to = 1;
      break;
    default:
      return -1;
  }

  if (fd_from == -1) {
    perror("open");
    return -1;
  }
  if (dup2(fd_from, fd_to) == -1) {
    perror("dup2");
    return -1;
  }

  bool need_close = redirect->type != REDIRECT_INPUT_DUP && redirect->type != REDIRECT_OUTPUT_DUP;
  if (need_close && close(fd_from) == -1) {
    perror("close");
    return -1;
  }

  return 0;
}

int exec_external_command(simple_command_t *command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    if (command->redirect && exec_redirect(command->redirect) == -1) {
      exit(1);
    }

    size_t argv_len = command->args_len + 2;
    char **argv = calloc(argv_len, sizeof(char *));
    argv[0] = command->file;
    argv[argv_len - 1] = NULL;
    for (size_t i = 0; i < command->args_len; i++) {
      argv[i + 1] = command->args[i];
    }

    if (execvp(command->file, argv) == -1) {
      perror("execvp");
      exit(1);
    }

    __builtin_unreachable();
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    return WEXITSTATUS(stat);
  }
}

int exec_simple_command(simple_command_t *simple_command) {
  int exit_code;
  if (!strcmp(simple_command->file, "cd")) {
    exit_code = exec_builtin_cd(simple_command);
  } else if (!strcmp(simple_command->file, "exec")) {
    exit_code = exec_builtin_exec(simple_command);
  } else if (!strcmp(simple_command->file, "exit")) {
    exec_builtin_exit(simple_command);
  } else {
    exit_code = exec_external_command(simple_command);
  }

  return exit_code;
}

int exec_list_command(list_command_t *list_command) {
  int last_exit_code = 0;

  list_command_t *cur = list_command;
  while (cur) {
    last_exit_code = exec_command(cur->command);

    switch (cur->type) {
      case LIST_SEQUENTIAL:
        break;
      case LIST_AND:
        if (last_exit_code != 0) return last_exit_code;
        break;
      case LIST_OR:
        if (last_exit_code == 0) return last_exit_code;
        break;
    }

    cur = cur->next;
  }

  return last_exit_code;
}

int exec_subshell_command(subshell_command_t *subshell_command) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    exec_command(subshell_command->command);
    exit(0);
  } else {
    int stat;
    waitpid(pid, &stat, 0);
    return WEXITSTATUS(stat);
  }
}

int exec_pipeline_inner(pipeline_command_t *pipeline) {
  if (pipeline->next == NULL) return exec_command(pipeline->command);

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    return 1;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    if (dup2(pipefd[1], 1) == -1) {
      perror("dup2");
      exit(1);
    }
    if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1) {
      perror("close");
      exit(1);
    }

    int exit_code = exec_command(pipeline->command);
    exit(exit_code);
  } else {
    if (dup2(pipefd[0], 0) == -1) {
      perror("dup2");
      return 1;
    }
    if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1) {
      perror("close");
      return 1;
    }

    return exec_pipeline_inner(pipeline->next);
  }
}

int exec_pipeline_command(pipeline_command_t *pipeline) {
  int last_exit_code;
  if (!pipeline->next) {
    last_exit_code = exec_command(pipeline->command);
  } else {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return 1;
    }

    if (pid == 0) {
      int exit_code = exec_pipeline_inner(pipeline);
      exit(exit_code);
    } else {
      int stat;
      waitpid(pid, &stat, 0);
      last_exit_code = WEXITSTATUS(stat);
    }
  }

  if (pipeline->negate_exit_code) {
    if (last_exit_code == 0) {
      last_exit_code = 1;
    } else {
      last_exit_code = 0;
    }
  }

  return last_exit_code;
}

int exec_command(command_t *command) {
  switch (command->type) {
    case COMMAND_SIMPLE:
      return exec_simple_command(command->value.simple);
    case COMMAND_LIST:
      return exec_list_command(command->value.list);
    case COMMAND_SUBSHELL:
      return exec_subshell_command(command->value.subshell);
    case COMMAND_PIPELINE:
      return exec_pipeline_command(command->value.pipeline);
  }
}
