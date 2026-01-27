#include "../../include/run/pipeline.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int run_pipeline(pipeline_t *pl) {
  printf("entering run_pipeline\n");
  int pfds[2];
  int prev_fd = -1;
  for (size_t i = 0; i < pl->count; i++) {
    if (i < pl->count - 1) {
      pipe(pfds);
    }
    pc cmd = pl->commands[i];
    pid_t pid = fork();
    if (i == 0) {
      // reads from stdin
      // emits to stdout however we want to redirect stdout to diff fd
      switch (pid) {
      case -1: {
        perror("fork");
        return -1;
      }
      case 0: {
        close(pfds[0]);
        dup2(pfds[1], STDOUT_FILENO);
        execvp(cmd.argv[0], cmd.argv);
        perror("execvp");
        return -1;
      }
      default: {
        printf("going to run %s\n", cmd.argv[0]);
      }
      }
    } else {
      switch (pid) {
      case -1: {
        perror("fork");
        return -1;
      }
      case 0: {
        // child process
        dup2(prev_fd, STDIN_FILENO);
        dup2(pfds[1], STDOUT_FILENO);
        close(prev_fd);
        prev_fd = pfds[0];
        execvp(cmd.argv[0], cmd.argv);
        perror("execvp");
        return -1;
      }
      default: {
        // parent process
      }
      }
    }
  }
  printf("exiting run_pipeline\n");
  return 0;
}
