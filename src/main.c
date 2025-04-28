#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_builtin(char *input) {

  if (strncmp(input, "echo", 4) == 0) {
    return 1;
  }
  if (strncmp(input, "exit", 4) == 0) {
    return 1;
  }
  if (strncmp(input, "type", 4) == 0) {
    return 1;
  }
  return 0;
}
void check_if_type_exists(char *input) {
  int found = 0;
  input = input + 5;
  if (is_builtin(input)) {
    printf("%s is a shell builtin\n", input);
    return;
  }
  char *path_variable = getenv("PATH");
  if (!path_variable) {
    perror("Path Vairable Not Found");
    return;
  }
  char *path_variable_copy = strdup(path_variable);
  char *token = strtok(path_variable_copy, ":");
  while (token != NULL) {
    char *file_with_path = malloc(
        (strlen(token) + strlen("/") + strlen(input) + 1) * sizeof(char));
    file_with_path[0] = '\0';
    strcat(file_with_path, token);
    strcat(file_with_path, "/");
    strcat(file_with_path, input);
    if (access(file_with_path, F_OK) == 0) {
      printf("%s is %s\n", input, file_with_path);
      found = 1;
      free(file_with_path);
      break;
    }
    free(file_with_path);
    token = strtok(NULL, ":");
  }
  free(path_variable_copy);
  if (!found) {
    printf("%s: not found\n", input);
  }
}

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Wait for user input
  char input[100];
  while (1) {
    printf("$ ");
    //**********************************************
    // extract input properly and remove new line character
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';
    //************************** handle exit 0 command ***************
    if (strcmp(input, "exit 0") == 0) {
      return 0;
    }
    //****************handle echo command****************************
    if (strncmp(input, "echo", 4) == 0) {
      printf("%s\n", input + 5);
      continue;
    }
    //***************handle type command ***************************
    if (strncmp(input, "type", 4) == 0) {
      check_if_type_exists(input);
      continue;
    }
    printf("%s: command not found\n", input);
  }
  return 0;
}
