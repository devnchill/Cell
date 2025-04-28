#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototype for getfilepath
char *getfilepath(char *file);

int execute_command(char *command) {
  // Get the full path of the command
  char *path = getfilepath(command);
  if (path) {
    // Split the command into arguments
    char *args[2];
    args[0] = path;
    args[1] = NULL; // If no additional arguments, set NULL for the last element

    // Execute the command using execvp, which uses the PATH environment
    // variable
    if (execvp(args[0], args) == -1) {
      perror("Error executing command");
      return -1; // Error executing the command
    }
  } else {
    printf("%s: command not found\n", command);
    return -1;
  }
  return 0;
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

    // Execute the command
    if (execute_command(input) == 0) {
      continue;
    }
  }

  return 0;
}
