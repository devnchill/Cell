#include "../../include/parse_command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
pc parse_command(char *command, size_t size) {
  pc p = {0};

  if (fgets(command, size, stdin) == NULL) {
    p.argv = NULL;
    return p;
  };
  command[strcspn(command, "\n")] = '\0';

  p.argv = malloc(128 * sizeof(char *));
  if (!p.argv)
    return p;

  char *token = strtok(command, " ");
  while (token && p.argc < 127) {
    p.argv[p.argc++] = token;
    token = strtok(NULL, " ");
  }
  p.argv[p.argc] = NULL;

  return p;
}
