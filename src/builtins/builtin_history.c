#include <stdio.h>

#include <readline/readline.h>
#include <stdlib.h>

#include <readline/history.h>

int builtin_history(int argc, char **argv) {
  HISTORY_STATE *hs = history_get_history_state();
  if (!hs || !hs->entries)
    return 0;
  int len = hs->length, n = len;

  if (argc == 3) {
    char *file_path = argv[2];
    if (0 == strcmp(argv[1], "-r")) {
      return read_history(file_path);
    }

    if (argc == 3 && strcmp(argv[1], "-w") == 0) {
      return write_history(file_path);
    }
  }

  if (argc == 2) {
    n = atoi(argv[1]);
    if (n <= 0) {
      fprintf(stderr, "history: invalid argument\n");
      return -1;
    }
    if (n > len) {
      fprintf(stdout, "history: showing all entries\n");
      goto PRINT_ALL;
    }
  }

  for (int i = len - n; i < len; i++) {
    printf("    %d  %s\n", i + 1, hs->entries[i]->line);
  }
  return 0;

PRINT_ALL:
  for (int i = 0; i < len; i++) {
    printf("    %d  %s\n", i + 1, hs->entries[i]->line);
  }
  return 0;
}
