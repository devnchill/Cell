#include <stdio.h>

#include <readline/readline.h>
#include <stdlib.h>

#include <readline/history.h>
int builtin_history(int argc, char **argv) {

  if (argc == 3) {
    char *file_path = argv[2];

    if (0 == strcmp(argv[1], "-r")) {
      return read_history(file_path);
    }

    if (0 == strcmp(argv[1], "-w")) {
      return write_history(file_path);
    }

    if (0 == strcmp(argv[1], "-a")) {
      return append_history(history_length - history_lines_written_to_file,
                            file_path);
    }
  }

  HISTORY_STATE *hs = history_get_history_state();
  int len = hs->length, n = len;

  if (!hs || !hs->entries)
    return 0;

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

  read_history(getenv("HISTFILE"));
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
