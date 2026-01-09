#ifndef TRIE
#define TRIE

#include <stdlib.h>

#include <stdbool.h>
#define NUM_CHAR 256

typedef struct trienode {
  bool terminal;
  struct trienode *children[NUM_CHAR];
} trienode;

trienode *create_node();

bool trie_insert(trienode **root, char *word);

trienode *trie_get_root();

bool trie_is_present(trienode **root, char *word);

void trie_init();

void trie_insert_word(char *word);

char **trie_get_matches(char *word);

#endif
