#include <stdio.h>
#include <dirent.h>   
#include <sys/types.h> 
#include <readline/history.h>
#include <readline/readline.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototype
char *getfilepath(char *fileName);
void print_cwd(void);
void check_if_type_exists(char *type);
int is_builtin(char *command);
int execute_command(char *command);

// Builtin commands
char *builtin_commands_array[] = {"echo", "type", "exit", "pwd", NULL};
#define MAX_EXTERNAL_CMDS 1024
char **external_commands_array = NULL;
int external_command_count = 0;

// Print current working directory
void print_cwd(void) {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("getcwd");
  }
}

// Check if input is a builtin command
int is_builtin(char *input) {
  for (int i = 0; builtin_commands_array[i] != NULL; i++) {
    if (strcmp(input, builtin_commands_array[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

// Search for command in PATH
char *getfilepath(char *file) {
  char *path = getenv("PATH");
  if (!path)
    return NULL;

  char *path_copy = strdup(path);
  if (!path_copy)
    return NULL;

  char *token = strtok(path_copy, ":");
  while (token != NULL) {
    char *full_path = malloc(strlen(token) + strlen(file) + 2);
    if (!full_path)
      break;

    sprintf(full_path, "%s/%s", token, file);
    if (access(full_path, F_OK) == 0) {
      free(path_copy);
      return full_path; // Caller must free
    }
    free(full_path);
    token = strtok(NULL, ":");
  }
  free(path_copy);
  return NULL;
}

// Check for "type" command behavior
void check_if_type_exists(char *input) {
  char *arg = input + 5; // skip "type "
  while (*arg == ' ')
    arg++; // trim spaces

  if (strlen(arg) == 0) {
    printf("type: missing argument\n");
    return;
  }

  if (is_builtin(arg)) {
    printf("%s is a shell builtin\n", arg);
    return;
  }

  char *filepath = getfilepath(arg);
  if (filepath) {
    printf("%s is %s\n", arg, filepath);
    free(filepath);
  } else {
    printf("%s: not found\n", arg);
  }
}

// Execute external command using system()
int execute_command(char *command) {
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
    int status = system(command);
    free(path);
    free(command_copy);
    return status == -1 ? -1 : 0;
  } else {
    printf("%s: command not found\n", first_word);
    free(command_copy);
    return -1;
  }
}

// For tab-completion
void populate_external_commands() {
    char *path = getenv("PATH");
    if (!path) return;

    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    int capacity = 256; // initial guess
    external_commands_array = malloc(capacity * sizeof(char *));

    while (dir) {
        DIR *dp = opendir(dir);
        if (dp) {
            struct dirent *entry;
            while ((entry = readdir(dp))) {
                char fullpath[PATH_MAX];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, entry->d_name);
                if (access(fullpath, X_OK) == 0 ) {
                    // Avoid duplicates
                    int duplicate = 0;
                    for (int i = 0; i < external_command_count; i++) {
                        if (strcmp(external_commands_array[i], entry->d_name) == 0) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (!duplicate) {
                        if (external_command_count >= capacity) {
                            capacity *= 2;
                            external_commands_array = realloc(external_commands_array, capacity * sizeof(char *));
                        }
                        external_commands_array[external_command_count++] = strdup(entry->d_name);
                    }
                }
            }
            closedir(dp);
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
}


char *command_generator(const char *text, int state) {
  static int list_index = 0;
  static int len = 0;
  static int in_builtin = 1;

  if (!state) {
    list_index = 0;
    len = strlen(text);
    in_builtin = 1;
  }

  if (in_builtin) {
    while (builtin_commands_array[list_index]) {
      const char *cmd = builtin_commands_array[list_index++];
      if (strncmp(cmd, text, len) == 0)
        return strdup(cmd);
    }
    in_builtin = 0;
    list_index = 0;
  }

  while (list_index < external_command_count) {
    const char *cmd = external_commands_array[list_index++];
    if (strncmp(cmd, text, len) == 0)
      return strdup(cmd);
  }

  return NULL;
}


char **command_completion(const char *text, int start, int end) {
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, command_generator);
}

// Main shell loop
int main() {
  setbuf(stdout, NULL);
  populate_external_commands();
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

    if (strcmp(line, "exit 0") == 0) {
      free(line);
      break;
    }

    if (strncmp(line, "echo", 4) == 0 && (line[4] == ' ' || line[4] == '\0')) {
      printf("%s\n", line + 5);
      free(line);
      continue;
    }

    if (strncmp(line, "type", 4) == 0 && (line[4] == ' ' || line[4] == '\0')) {
      check_if_type_exists(line);
      free(line);
      continue;
    }

    if (strcmp(line, "pwd") == 0) {
      print_cwd();
      free(line);
      continue;
    }

    execute_command(line);
    free(line);
  }

  for (int i = 0; i < external_command_count; i++) {
    free(external_commands_array[i]);
  }

  free(external_commands_array);
  return 0;
}
