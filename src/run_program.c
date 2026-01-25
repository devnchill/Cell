#include "../include/parser/parser.h"
#include "../include/redirect/stderr.h"
#include "../include/redirect/stdout.h"
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
    if (cmd->redirs.stderr_file)
      redirect_stderr(cmd, &(int){-1});

    if (cmd->redirs.stdout_file)
      redirect_stdout(cmd, &(int){-1});

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
