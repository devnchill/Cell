#include "../../include/parser/parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void redirect_stdout(pc *command) {
  int fd =
      open(command->redirs.stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open stdout");
    return;
  }

  // stderr is closed and instead now it points to file passed
  if (dup2(fd, STDOUT_FILENO) < 0) {
    perror("dup2 stdout");
    _exit(1);
  };
  close(fd);
}
