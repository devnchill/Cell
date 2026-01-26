#include "../../include/parser/parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void redirect_stderr(pc *command) {
  int fd = -1;
  if (command->redirs.stderr_append)
    fd =
        open(command->redirs.stderr_file, O_WRONLY | O_CREAT | O_APPEND | 0644);
  else
    fd = open(command->redirs.stderr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open stderr");
    return;
  }

  // stdout is closed and instead now it points to file passed
  if (dup2(fd, STDERR_FILENO) < 0) {
    perror("dup2 stderr");
    _exit(1);
  };
  close(fd);
}
