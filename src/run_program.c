#include "../include/is_present_in_path.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int run_program(int argc, char **argv) {
  char *executable = argv[0];
  executable_in_system_path eisp = executable_is_present(executable);
  if (eisp.is_present) {
    pid_t pid = fork();
    switch (pid) {
    case -1: {
      perror("failed starting child process");
    };
    case 0: {
      execvp(executable, argv);
      break;
    };
    default: {
      waitpid(pid, NULL, 0);
      break;
    };
    }
    return 0;
  }
  return -1;
}
