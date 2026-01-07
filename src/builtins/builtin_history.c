#include <stdio.h>

#include <readline/history.h>
int builtin_history(int argc, char **argv) {
  typedef void *histdata_t;
  HIST_ENTRY **history = history_list();
  for (size_t i = 0; history[i]; i++) {
    printf("    %zu  %s\n", i + 1, history[i]->line);
  }
  return 0;
}
