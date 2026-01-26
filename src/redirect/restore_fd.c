#include <unistd.h>
void restore_fd(int saved_stdout, int saved_stderr) {

  if (saved_stdout != -1) {
    // point fd of stdout back to itself
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
  }

  if (saved_stderr != -1) {
    // point fd of stderr back to itself
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
  }
}
