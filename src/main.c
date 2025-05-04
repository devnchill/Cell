#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototype for getfilepath
char *getfilepath(char *file);
char *builtin_commands_array[] = {"echo", "type", "exit", "pwd"};

void print_cwd(char *input) {
  char cwd[200];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    printf("");
  }
}

int execute_command(char *command) {
  // Create a copy of the command to extract the first word
  char *command_copy = strdup(command);
  if (!command_copy) {
    perror("strdup");
    return -1;
  }

  char *first_word = strtok(command_copy, " ");
  if (!first_word) {
    free(command_copy);
    return -1;
  }

  char *path = getfilepath(first_word);
  if (path) {
    if (system(command) == -1) {
      free(path);
      free(command_copy);
      return -1; // Error executing the command
    }
    free(path);
  } else {
    printf("%s: command not found\n", first_word);
    free(command_copy);
    return -1;
  }

  free(command_copy);
  return 0;
}

int is_builtin(char *input) {
  int num_commands =
      sizeof(builtin_commands_array) / sizeof(builtin_commands_array[0]);

  for (int i = 0; i < num_commands; i++) {
    if (strcmp(input, builtin_commands_array[i]) == 0) {
      return 1;
    }
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

char *command_generator(const char *text, int state) {
  static int list_index;
  static int len;

  if (!state) {
    list_index = 0;
    len = strlen(text);
  }

  while (builtin_commands_array[list_index]) {
    const char *cmd = builtin_commands_array[list_index];
    list_index++;

    if (strncmp(cmd, text, len) == 0) {
      return strdup(cmd);
    }
  }

  return NULL;
}

char **command_completion(const char *text, int start, int end) {
  rl_attempted_completion_over = 1; // Prevent filename completion
  return rl_completion_matches(text, command_generator);
}

int main() {
  setbuf(stdout, NULL); // Flush after every printf

  char input[100];
  rl_attempted_completion_function = command_completion;
  while (1) {
    char *line = readline("\x1b[0;32m$\x1b[0m ");
    if (!line)
      break;

    if (strlen(line) == 0) {
      free(line);
      continue;
    }
    add_history(line);
    strcpy(input, line);
    free(line);

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

    // Handle pwd command
    if (strcmp(input, "pwd") == 0) {
      print_cwd(input);
      continue;
    }
    // Execute the command
    if (execute_command(input) == 0) {
      continue;
    }
  }

  return 0;
}
