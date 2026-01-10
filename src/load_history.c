#include <stdio.h>

#include <readline/history.h>
#include <stdlib.h>
void load_history() {
  char *file = getenv("HISTFILE");
  if (!file)
    return;
  read_history(file);
}
