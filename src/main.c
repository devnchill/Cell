#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Function prototype
char *getfilepath(char *fileName);
void print_cwd(void);
void check_if_type_exists(char *type);
int is_builtin(char *command);
void print_history(char *line);
void change_directory(char *line);
void execute_command(char *command);
char *trim_whitespace(char *line);

// Builtin commands
char *builtin_commands_array[] = {"echo", "cd",  "history", "type",
                                  "exit", "pwd", NULL};
#define MAX_EXTERNAL_CMDS 1024
char **external_commands_array = NULL;
int external_command_count = 0;

typedef struct {
  char **cmd_args;
  char *redir_file;
  int redir_fd;
  int append;
} parsed_command_t;

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
      return full_path;
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
    arg++;

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

parsed_command_t parse_redirection(char **argv) {
  parsed_command_t result = {NULL, NULL, 0, 0}; // default append = 0
  int i = 0;
  while (argv[i] != NULL) {
    if (strcmp(argv[i], ">>") == 0 || strcmp(argv[i], "1>>") == 0) {
      result.redir_fd = 1;
      result.append = 1;
      break;
    } else if (strcmp(argv[i], "2>>") == 0) {
      result.redir_fd = 2;
      result.append = 1;
      break;
    } else if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], "1>") == 0) {
      result.redir_fd = 1;
      result.append = 0;
      break;
    } else if (strcmp(argv[i], "2>") == 0) {
      result.redir_fd = 2;
      result.append = 0;
      break;
    }
    i++;
  }
  result.cmd_args = malloc((i + 1) * sizeof(char *));
  for (int j = 0; j < i; j++) {
    result.cmd_args[j] = argv[j];
  }
  result.cmd_args[i] = NULL;
  if (argv[i] != NULL && argv[i + 1] != NULL) {
    result.redir_file = argv[i + 1];
  }
  return result;
}

char **parse_cmd(char *command) {
  int capacity = 8;
  char **argv = malloc(capacity * sizeof(char *));
  int argc = 0;

  char *arg_buf = malloc(1024);
  int len = 0;

  int in_single = 0, in_double = 0;

  for (char *p = command; *p; p++) {
    char c = *p;

    if (in_single) {
      if (c == '\'') {
        in_single = 0;
      } else {
        arg_buf[len++] = c;
      }
    } else if (in_double) {
      if (c == '"') {
        in_double = 0;
      } else if (c == '\\') {
        p++;
        if (*p == '"' || *p == '\\' || *p == '$' || *p == '`') {
          arg_buf[len++] = *p;
        } else {
          arg_buf[len++] = '\\';
          if (*p)
            arg_buf[len++] = *p;
        }
      } else {
        arg_buf[len++] = c;
      }
    } else {
      if (isspace(c)) {
        if (len > 0) {
          arg_buf[len] = '\0';
          argv[argc++] = strdup(arg_buf);
          len = 0;
          if (argc >= capacity) {
            capacity *= 2;
            argv = realloc(argv, capacity * sizeof(char *));
          }
        }
      } else if (c == '\'') {
        in_single = 1;
      } else if (c == '"') {
        in_double = 1;
      } else if (c == '\\') {
        p++;
        if (*p)
          arg_buf[len++] = *p;
      } else {
        arg_buf[len++] = c;
      }
    }
  }

  if (len > 0) {
    arg_buf[len] = '\0';
    argv[argc++] = strdup(arg_buf);
  }

  free(arg_buf);
  argv[argc] = NULL;
  return argv;
}

void execute_pipeline(char *cmdline) {
  // Count number of pipes
  int count = 1;
  for (char *p = cmdline; *p; p++) {
    if (*p == '|')
      count++;
  }

  int pipefd[2 * (count - 1)];
  for (int i = 0; i < count - 1; i++) {
    if (pipe(pipefd + i * 2) < 0) {
      perror("pipe");
      return;
    }
  }

  int i = 0;
  char *cmd = strtok(cmdline, "|");
  while (cmd) {
    cmd = trim_whitespace(cmd);
    char **argv = parse_cmd(cmd);
    if (!argv || !argv[0]) {
      free(argv);
      break;
    }
    pid_t pid = fork();
    if (pid == 0) {
      // Setup pipe read from previous pipe (if not first)
      if (i > 0) {
        if (dup2(pipefd[(i - 1) * 2], STDIN_FILENO) < 0) {
          perror("dup2");
          exit(1);
        }
      }

      // Setup pipe write to next pipe (if not last)
      if (i < count - 1) {
        if (dup2(pipefd[i * 2 + 1], STDOUT_FILENO) < 0) {
          perror("dup2");
          exit(1);
        }
      }

      // Close all pipe fds in child
      for (int j = 0; j < 2 * (count - 1); j++) {
        close(pipefd[j]);
      }

      if (is_builtin(argv[0])) {
        if (strcmp(argv[0], "echo") == 0) {
          for (int k = 1; argv[k]; k++) {
            printf("%s", argv[k]);
            if (argv[k + 1])
              printf(" ");
          }
          printf("\n");
          exit(0);
        } else if (strcmp(argv[0], "pwd") == 0) {
          print_cwd();
          exit(0);
        } else if (strcmp(argv[0], "type") == 0) {
          // Use full command string because check_if_type_exists expects it
          check_if_type_exists(cmd);
          exit(0);
        } else if (strcmp(argv[0], "history") == 0) {
          print_history(cmd);
          exit(0);
        } else if (strcmp(argv[0], "cd") == 0) {
          change_directory(cmd);
          exit(0);
        } else if (strcmp(argv[0], "exit") == 0) {
          // exit builtin in pipeline child: just exit child
          exit(0);
        }
        exit(0);
      } else {
        execvp(argv[0], argv);
        perror("execvp");
        exit(1);
      }
    }

    // Parent continues
    for (int k = 0; argv[k]; k++)
      free(argv[k]);
    free(argv);

    cmd = strtok(NULL, "|");
    i++;
  }

  // Close all pipe fds in parent
  for (int j = 0; j < 2 * (count - 1); j++) {
    close(pipefd[j]);
  }

  // Wait for all children
  for (int j = 0; j < count; j++) {
    wait(NULL);
  }
}

// Execute external command
void execute_command(char *command) {
  if (strchr(command, '|')) {
    // Make a copy since strtok modifies string
    char *cmd_copy = strdup(command);
    execute_pipeline(cmd_copy);
    free(cmd_copy);
    return;
  }
  char **argv = parse_cmd(command);
  if (!argv)
    return;
  parsed_command_t parsed = parse_redirection(argv);
  char *cmd = parsed.cmd_args[0];
  if (!cmd) {
    free(argv);
    free(parsed.cmd_args);
    return;
  }

  int is_builtin_cmd = is_builtin(cmd);

  int saved_fd = -1;
  int fd = -1;
  if (parsed.redir_fd != 0 && parsed.redir_file != NULL) {
    int flags = O_WRONLY | O_CREAT;
    flags |= parsed.append ? O_APPEND : O_TRUNC;
    fd = open(parsed.redir_file, flags, 0644);
    if (fd < 0) {
      perror("open");
      goto cleanup;
    }
    saved_fd = dup(parsed.redir_fd);
    dup2(fd, parsed.redir_fd);
    close(fd);
  }

  if (is_builtin_cmd) {
    // Handle builtin commands
    if (strcmp(cmd, "echo") == 0) {
      for (int i = 1; parsed.cmd_args[i]; i++) {
        printf("%s", parsed.cmd_args[i]);
        if (parsed.cmd_args[i + 1])
          printf(" ");
      }
      printf("\n");
    } else if (strcmp(cmd, "pwd") == 0) {
      print_cwd();
    } else if (strcmp(cmd, "type") == 0) {
      check_if_type_exists(command);
    } else if (strcmp(cmd, "history") == 0) {
      print_history(command);
    } else if (strcmp(cmd, "cd") == 0) {
      change_directory(command);
    }
  } else {
    // External command
    int pid = fork();
    if (pid == 0) {
      execvp(parsed.cmd_args[0], parsed.cmd_args);
      fprintf(stderr, "%s: command not found\n", parsed.cmd_args[0]);
      _exit(1);
    } else {
      waitpid(pid, NULL, 0);
    }
  }

  // Restore stdout/stderr
  if (saved_fd != -1) {
    dup2(saved_fd, parsed.redir_fd);
    close(saved_fd);
  }

cleanup:
  for (int i = 0; argv[i] != NULL; i++)
    free(argv[i]);
  free(argv);
  free(parsed.cmd_args);
}

// For tab-completion
void populate_external_commands() {
  char *path = getenv("PATH");
  if (!path)
    return;

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
        if (access(fullpath, X_OK) == 0) {
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
              external_commands_array =
                  realloc(external_commands_array, capacity * sizeof(char *));
            }
            external_commands_array[external_command_count++] =
                strdup(entry->d_name);
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

void print_history(char *line) {
  HIST_ENTRY **hist_list = history_list();
  int n = -1;
  int count = 0;
  int start = 0;
  if (strlen(line) > 7) {
    char *no_of_cmds_to_display = line + 7;
    while (*no_of_cmds_to_display == ' ')
      no_of_cmds_to_display++;
    if (*no_of_cmds_to_display != '\0')
      n = atoi(no_of_cmds_to_display);
  }
  if (!hist_list)
    return;
  while (hist_list[count])
    count++;
  if (n < count && n > 0) {
    start = count - n;
  }
  for (int i = start; i < count; i++) {
    printf("    %d %s\n", i + history_base, hist_list[i]->line);
  }
}

void change_directory(char *line) {
  char *path = line + 2;
  int i = 0;
  while (*path == ' ')
    path++;
  if (*path == '\0')
    return;
  if (strcmp(path, "~") == 0) {
    path = getenv("HOME");
    if (!path) {
      fprintf(stderr, "cd: HOME environment variable not set\n");
      return;
    }
  }
  if (chdir(path) != 0)
    fprintf(stderr, "cd: %s: No such file or directory\n", path);
}

char *trim_whitespace(char *line) {
  // trime leading whitespace
  while (*line == ' ')
    line++;
  char *end = line + strlen(line) - 1;
  while (end >= line && *end == ' ') {
    *end = '\0';
    end--;
  }
  return line;
}

void write_to_file(char *cmd, char *filepath) {
  FILE *pipe_fp = popen(cmd, "r");
  if (pipe_fp == NULL) {
    printf("Failed to run command %s\n", cmd);
    return;
  }
  FILE *file_fp = fopen(filepath, "w");
  if (file_fp == NULL) {
    printf("Failed to open file %s for writing\n", filepath);
    pclose(pipe_fp);
    return;
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), pipe_fp) != NULL) {
    fputs(buffer, file_fp);
  }

  fclose(file_fp);
  pclose(pipe_fp);
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

    execute_command(line);
    free(line);
  }

  for (int i = 0; i < external_command_count; i++) {
    free(external_commands_array[i]);
  }

  free(external_commands_array);
  return 0;
}
