#include "../include/parser/parser.h"
#include "../include/redirect/stderr.h"
#include "../include/redirect/stdout.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int run_program(pc *cmd) {
  pid_t pid = fork();
  switch (pid) {
  case -1: {
    perror("fork");
    return -1;
  };
  case 0: {
    if (cmd->redirs.stderr_file)
      redirect_stderr(cmd);

    if (cmd->redirs.stdout_file)
      redirect_stdout(cmd);

    execvp(cmd->argv[0], cmd->argv);
    perror(cmd->argv[0]);
    _exit(127);
  };
  default: {
    int status = -1;
    if (waitpid(pid, &status, 0) < 0) {
      perror("waitpid");
      return -1;
    }
    if (WIFEXITED(status))
      return WEXITSTATUS(status);

    return -1;
  };
  }
}
