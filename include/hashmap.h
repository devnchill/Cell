#ifndef HASHMAP
#define HASHMAP

#include "shell_builtin.h"
#include <stdbool.h>

typedef struct {
  bool is_present;
  unsigned int index;
} get_index;

void hashmap_add(char *key, shell_builtin val);
get_index hashmap_is_present(char *key);
shell_builtin *hashmap_get(char *key);

#endif
