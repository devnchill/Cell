#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int run_program(int argc, char **argv) {
  pid_t pid = fork();
  switch (pid) {
  case -1: {
    perror("failed starting child process");
    return -1;
  };
  case 0: {
    execvp(argv[0], argv);
    return -1;
  };
  default: {
    waitpid(pid, NULL, 0);
    return 0;
  };
  }
}
