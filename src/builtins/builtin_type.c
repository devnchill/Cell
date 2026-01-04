#include "../../include/hashmap.h"
#include "../../include/shell_builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int builtin_type(int argc, char **argv) {
  char *cmd = argv[1];
  shell_builtin *builtin = hashmap_get(cmd);
  if (builtin != NULL) {
    printf("%s is a shell builtin\n", cmd);
    return 0;
  }
  char *path = getenv("PATH");
  if (path == NULL) {
    printf("path not found\n");
    return -1;
  }
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", token, cmd);
    if (access(full_path, X_OK) == 0) {
      printf("%s is %s\n", cmd, full_path);
      free(path_copy);
      return 0;
    }
    token = strtok(NULL, ":");
  }
  free(path_copy);
  printf("%s: not found\n", cmd);
  return 0;
}
