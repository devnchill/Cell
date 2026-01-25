#include "../include/parse_command.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int run_program(pc *cmd) {
  pid_t pid = fork();
  switch (pid) {
  case -1: {
    perror("failed starting child process");
    return -1;
  };
  case 0: {
    if (cmd->redirs.stdout_file) {
      int fd =
          open(cmd->redirs.stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        perror("open");
        _exit(1);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    execvp(cmd->argv[0], cmd->argv);
    fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
    _exit(127);
  };
  default: {
    waitpid(pid, NULL, 0);
    return 0;
  };
  }
}
