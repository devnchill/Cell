#include <stdio.h>

#include "../include/trie.h"
#include <readline/readline.h>

#define NUM_BUILTINS 2

void setup_autocomplete_for_builtins() {
  char *builtins[NUM_BUILTINS] = {"echo", "exit"};
  for (size_t i = 0; i < NUM_BUILTINS; i++) {
    trie_insert_word(builtins[i]);
  }
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
    return rl_completion_matches(text, rl_filename_completion_function);
  }
  return rl_completion_matches(text, rl_filename_completion_function);
}

void init_auto_completion(trienode *root) {
  trie_init();
  setup_autocomplete_for_builtins();
  rl_attempted_completion_function = command_completion;
  rl_complete(0, '\t');
}
