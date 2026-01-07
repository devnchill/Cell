#include "../../include/tokenize_command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tc tokenize_command(char *command, size_t size) {
  tc t = {0};

  if (fgets(command, size, stdin) == NULL) {
    t.argv = NULL;
    return t;
  };
  command[strcspn(command, "\n")] = '\0';

  t.argv = malloc(128 * sizeof(char *));
  if (!t.argv)
    return t;

  char *token = strtok(command, " ");
  while (token && t.argc < 127) {
    t.argv[t.argc++] = token;
    token = strtok(NULL, " ");
  }
  t.argv[t.argc] = NULL;

  return t;
}
