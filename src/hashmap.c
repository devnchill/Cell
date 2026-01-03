#include "../include/hashmap.h"
#include "../include/shell_builtin.h"
#include <stdbool.h>
#include <string.h>

typedef struct {
  char *key;
  shell_builtin val;
} entry;

static entry arr[100];

unsigned int hash(const char *str) {
  unsigned long hash = 3815;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash % 100;
}

// NOTE: might have collision , refactor it later
void hashmap_add(char *key, shell_builtin val) {
  int index = hash(key);
  arr[index].key = strdup(key);
  arr[index].val = val;
}

get_index hashmap_is_present(char *key) {
  unsigned int index = hash(key);
  get_index g;
  if (arr[index].key != NULL && strcmp(arr[index].key, key) == 0) {
    g.index = index;
    g.is_present = true;
    return g;
  }
  g.is_present = false;
  g.index = -1;
  return g;
}

shell_builtin *hashmap_get(char *key) {
  get_index g = hashmap_is_present(key);
  if (g.is_present) {
    return &arr[g.index].val;
  }
  return NULL;
}
