#include "../../include/helpers/is_present_in_path.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

executable_in_system_path executable_is_present(char *executable_name) {
  const char *system_path = getenv("PATH");
  if (!system_path) {
    perror("couldn't get path from environment");
    return (executable_in_system_path){.is_present = false};
  }

  char *system_path_copy = strdup(getenv("PATH"));
  if (!system_path_copy) {
    return (executable_in_system_path){.is_present = false};
  }

  char *token = strtok(system_path_copy, ":");

  while (token) {
    char tmp_path[1024];
    snprintf(tmp_path, sizeof(tmp_path), "%s/%s", token, executable_name);

    if (access(tmp_path, X_OK) == 0) {
      executable_in_system_path result = {.is_present = true};
      strcpy(result.full_path, tmp_path);
      free(system_path_copy);
      return result;
    }

    token = strtok(NULL, ":");
  }

  free(system_path_copy);
  return (executable_in_system_path){.is_present = false};
}
