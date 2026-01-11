#include "../../include/tokenize_command.h"
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

tc tokenize_command() {
  tc t = {0};
  char *command = readline("$ ");
  add_history(command);

  t.argv = malloc(128 * sizeof(char *));
  if (!t.argv)
    return t;

  char *token = strtok(command, " ");
  while (token && t.argc < 127) {
    if (0 == strcmp(">", token) || 0 == strcmp("1>", token)) {
      token = strtok(NULL, " ");
      if (!token) {
        fprintf(stderr, "syntax error: exptected file\n");
        break;
      }
      t.stdout_file = token;
    } else {
      t.argv[t.argc++] = token;
    }
    token = strtok(NULL, " ");
  }
  t.argv[t.argc] = NULL;

  return t;
}
