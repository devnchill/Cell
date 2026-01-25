#include "../../include/parser/parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void redirect_stderr(pc *command, int *saved_fd) {
  int fd =
      open(command->redirs.stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open");
    free(command->argv);
    return;
  }

  // we store the fd of stdout
  *saved_fd = dup(STDERR_FILENO);

  // stdout is closed and instead now it points to file passed
  dup2(fd, STDERR_FILENO);
  close(fd);
}
