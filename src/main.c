#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototype for getfilepath
char *getfilepath(char *file);

int execute_command(char *command) {
  char *path = getfilepath(command);
  if (path) {
    printf("Current PATH: %s\n", getenv("PATH"));
    if (system(command) == -1) {
      return -1; // Error executing the command
    }
  } else {
    printf("%s: command not found\n", command);
    return -1;
  }
  return 0;
}

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
  input = input + 5; // Skip "type "

  if (is_builtin(input)) {
    printf("%s is a shell builtin\n", input);
    return;
  }

  // Get the PATH environment variable
  char *path_variable = getenv("PATH");
  if (!path_variable) {
    perror("Path Variable Not Found");
    return;
  }

  // Copy the PATH variable to avoid modifying the original
  char *path_variable_copy = strdup(path_variable);
  char *token = strtok(path_variable_copy, ":");

  while (token != NULL) {
    char *file_with_path =
        malloc(strlen(token) + strlen("/") + strlen(input) + 1);
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

char *getfilepath(char *file) {
  char *path = getenv("PATH");
  if (!path)
    return NULL;
  char *path_copy = strdup(path);
  char *token = strtok(path_copy, ":");
  while (token != NULL) {
    char *full_path = malloc(strlen(token) + strlen(file) + 2);
    sprintf(full_path, "%s/%s", token, file);
    if (access(full_path, F_OK) == 0) {
      free(path_copy);
      return full_path;
    }
    free(full_path);
    token = strtok(NULL, ":");
  }
  free(path_copy);
  return NULL;
}

int main() {
  setbuf(stdout, NULL); // Flush after every printf

  char input[100];
  while (1) {
    printf("\x1b[0;32m$\x1b[0m ");
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0'; // Remove the newline character

    // Handle exit 0 command
    if (strcmp(input, "exit 0") == 0) {
      return 0;
    }

    // Handle echo command
    if (strncmp(input, "echo", 4) == 0) {
      printf("%s\n", input + 5);
      continue;
    }

    // Handle type command
    if (strncmp(input, "type", 4) == 0) {
      check_if_type_exists(input);
      continue;
    }

    // Execute the command
    if (execute_command(input) == 0) {
      continue;
    }
  }

  return 0;
}
