#include "../../include/run/pipeline.h"
#include "../../include/helpers/hashmap.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int run_pipeline(pipeline_t *pl) {
  int pfds[2];
  int prev_fd = -1;

  for (size_t i = 0; i < pl->count; i++) {
    if (i < pl->count - 1)
      pipe(pfds);

    pc cmd = pl->commands[i];
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      return -1;
    }

    if (pid == 0) {
      if (prev_fd != -1) {
        dup2(prev_fd, STDIN_FILENO);
      }
      if (i < pl->count - 1) {
        dup2(pfds[1], STDOUT_FILENO);
      }
      if (pfds[0])
        close(pfds[0]);
      if (pfds[1])
        close(pfds[1]);
      if (prev_fd != -1)
        close(prev_fd);

      shell_builtin *builtin = hashmap_get(cmd.argv[0]);
      if (builtin) {
        builtin->func(cmd.argc, cmd.argv);
        _exit(0);
      }

      execvp(cmd.argv[0], cmd.argv);
      perror("execvp");
      _exit(1);
      execvp(cmd.argv[0], cmd.argv);
      perror("execvp");
      _exit(1);
    }

    if (prev_fd != -1)
      close(prev_fd);
    if (i < pl->count - 1) {
      close(pfds[1]);
      prev_fd = pfds[0];
    }
  }
  for (size_t i = 0; i < pl->count; i++) {
    wait(NULL);
  }
  return 0;
}
