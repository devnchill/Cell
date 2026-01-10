#include <stdio.h>

#include <readline/history.h>
#include <stdlib.h>
int builtin_exit(int argc, char **argv) {
  char *file = getenv("HISTFILE");
  write_history(file);
  exit(0);
}
