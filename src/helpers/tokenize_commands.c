#include "../../include/tokenize_command.h"
#include <stdbool.h>
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

tc tokenize_command() {
  tc t = {0};
  char *line = readline("$ ");
  if (!line)
    return t;
  add_history(line);

  t.argv = malloc(128 * sizeof(char *));
  if (!t.argv)
    return t;

  char buf[1024];
  int bi = 0;
  bool in_quote = 0;

  for (int i = 0;; i++) {
    char c = line[i];
    if (c == '"') {
      in_quote = !in_quote;
      continue;
    }
    if ((c == ' ' || c == '\0') && !in_quote) {
      if (bi > 0) {
        buf[bi] = '\0';
        t.argv[t.argc++] = strdup(buf);
        bi = 0;
      }
      if (c == '\0') {
        break;
      }
      continue;
    }
    buf[bi++] = c;
  }

  t.argv[t.argc] = NULL;
  free(line);
  return t;
}
