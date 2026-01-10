#include <stdio.h>

#include <readline/readline.h>
#include <stdlib.h>

#include <readline/history.h>

int builtin_history(int argc, char **argv) {
  if (argc == 3) {
    if (strcmp(argv[1], "-r") == 0)
      return read_history(argv[2]);

    if (strcmp(argv[1], "-w") == 0)
      return write_history(argv[2]);

    if (strcmp(argv[1], "-a") == 0)
      return append_history(history_length - history_lines_written_to_file,
                            argv[2]);
  }

  HISTORY_STATE *hs = history_get_history_state();
  if (!hs || !hs->entries)
    return 0;

  int len = hs->length;
  int n = len;

  if (argc == 2) {
    n = atoi(argv[1]);
    if (n <= 0) {
      fprintf(stderr, "history: invalid argument\n");
      return -1;
    }
    if (n > len)
      n = len;
  }

  for (int i = len - n; i < len; i++) {
    printf("    %d  %s\n", i + 1, hs->entries[i]->line);
  }

  return 0;
}
