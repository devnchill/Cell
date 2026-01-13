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

  bool single_quote = false;
  bool double_quote = false;

  for (int i = 0;; i++) {
    char c = line[i];

    if (c == '\\') {
      if (single_quote) {
        buf[bi++] = '\\';
      } else if (double_quote) {
        char next = line[i + 1];
        if (next == '$' || next == '"' || next == '\\' || next == '\n') {
          i++;
          buf[bi++] = next;
        } else {
          buf[bi++] = '\\';
        }
      } else {
        if (line[i + 1] == '\0') {
        } else {
          i++;
          buf[bi++] = line[i];
        }
      }
      continue;
    }

    if (!single_quote && !double_quote && c == '1' && line[i + 1] == '>') {
      if (bi > 0) {
        buf[bi] = '\0';
        t.argv[t.argc++] = strdup(buf);
        bi = 0;
      }

      i += 2;

      while (line[i] == ' ')
        i++;

      char filebuf[1024];
      int fi = 0;
      while (line[i] && line[i] != ' ') {
        filebuf[fi++] = line[i++];
      }
      filebuf[fi] = '\0';

      t.stdout_file = strdup(filebuf);
      i--;
      continue;
    }
    if (!single_quote && !double_quote && c == '>') {
      if (bi > 0) {
        buf[bi] = '\0';
        t.argv[t.argc++] = strdup(buf);
        bi = 0;
      }

      i++;
      while (line[i] == ' ')
        i++;

      char filebuf[1024];
      int fi = 0;
      while (line[i] && line[i] != ' ') {
        filebuf[fi++] = line[i++];
      }
      filebuf[fi] = '\0';

      t.stdout_file = strdup(filebuf);
      i--;
      continue;
    }
    if (c == '\'' && !double_quote) {
      single_quote = !single_quote;
      continue;
    }

    if (c == '"' && !single_quote) {
      double_quote = !double_quote;
      continue;
    }
    if ((c == ' ' || c == '\0') && !single_quote && !double_quote) {
      if (bi > 0) {
        buf[bi] = '\0';
        t.argv[t.argc++] = strdup(buf);
        bi = 0;
      }
      if (c == '\0')
        break;
      continue;
    }
    buf[bi++] = c;
  }

  t.argv[t.argc] = NULL;
  free(line);
  return t;
}
