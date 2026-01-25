#include <stdio.h>

#include "../include/helpers/trie.h"
#include <dirent.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define NUM_BUILTINS 2

void setup_autocomplete_for_builtins() {
  char *builtins[NUM_BUILTINS] = {"echo", "exit"};
  for (size_t i = 0; i < NUM_BUILTINS; i++) {
    trie_insert_word(builtins[i]);
  }
}

void setup_autocomplete_for_path_executables() {
  const char *system_path = getenv("PATH");
  if (!system_path)
    return;
  char *path_copy = strdup(system_path);
  char *dir_path = strtok(path_copy, ":");
  while (dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
      dir_path = strtok(NULL, ":");
      continue;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_name[0] == '.')
        continue;
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
      if (access(full_path, X_OK) == 0) {
        trie_insert_word(entry->d_name);
      }
    }
    closedir(dir);
    dir_path = strtok(NULL, ":");
  }
  free(path_copy);
}

char *command_generator(const char *text, int state) {
  static char **matches = NULL;
  static int index = 0;
  if (state == 0) {
    index = 0;
    if (matches) {
      for (int i = 0; matches[i]; i++)
        free(matches[i]);
      free(matches);
    }
    matches = trie_get_matches((char *)text);
  }
  if (matches && matches[index]) {
    return strdup(matches[index++]);
  }
  return NULL;
}

char **command_completion(const char *text, int start, int end) {
  (void)start;
  (void)end;
  if (start == 0) {
    char **matches = rl_completion_matches(text, command_generator);
    if (matches)
      return matches;
  }
  return rl_completion_matches(text, rl_filename_completion_function);
}

void init_auto_completion(trienode *root) {
  trie_init();
  setup_autocomplete_for_builtins();
  setup_autocomplete_for_path_executables();
  rl_attempted_completion_function = command_completion;
  rl_complete(0, '\t');
}
