#include "../../include/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

trienode *create_node() {
  trienode *node = malloc(sizeof(*node));
  if (!node) {
    fprintf(stderr, "Error creating new node");
    return NULL;
  }
  for (size_t i = 0; i < NUM_CHAR; i++) {
    node->children[i] = NULL;
  }
  node->terminal = false;
  return node;
}

bool trie_insert(trienode **root, char *word) {
  if (*root == NULL)
    *root = create_node();
  trienode *temp = *root;
  int len = strlen(word);
  for (size_t i = 0; i < len; i++) {
    int idx = (unsigned char)word[i] - 'a';
    if (!temp->children[idx]) {
      temp->children[idx] = create_node();
    }
    temp = temp->children[idx];
  }
  if (temp->terminal)
    return false;
  temp->terminal = true;
  return true;
}

bool trie_is_present(trienode **root, char *word) {
  if (*root == NULL)
    return false;
  trienode *temp = *root;
  int len = strlen(word);
  for (size_t i = 0; i < len; i++) {
    if (!temp->children[word[i]]) {
      return false;
    }
    temp = temp->children[word[i]];
  }
  if (temp->terminal)
    return true;
  return false;
}

static trienode *root = NULL;

void trie_init() { root = create_node(); }

trienode *trie_get_root() { return root; }

void trie_insert_word(char *word) { trie_insert(&root, word); }

void trie_collect(trienode *node, char *prefix, size_t prefix_len,
                  char ***matches, size_t *count, size_t *capacity) {
  if (!node)
    return;
  if (node->terminal) {
    if (*count >= *capacity) {
      *capacity *= 2;
      *matches = realloc(*matches, (*capacity) * sizeof(char *));
    }
    char *word = malloc(prefix_len + 1);
    strncpy(word, prefix, prefix_len);
    word[prefix_len] = '\0';
    (*matches)[*count] = word;
    (*count)++;
  }

  for (int i = 0; i < NUM_CHAR; i++) {
    if (node->children[i]) {
      prefix[prefix_len] = (char)(i + 'a');
      prefix[prefix_len + 1] = '\0';
      trie_collect(node->children[i], prefix, prefix_len + 1, matches, count,
                   capacity);
    }
  }
}

char **trie_get_matches(char *word) {
  trienode *root_ptr = trie_get_root();
  if (!root_ptr) {
    char **empty = malloc(sizeof(char *));
    empty[0] = NULL;
    return empty;
  }

  trienode *temp = root_ptr;
  size_t len = strlen(word);

  for (size_t i = 0; i < len; i++) {
    int idx = (unsigned char)word[i] - 'a';
    if (!temp->children[idx]) {
      char **empty = malloc(sizeof(char *));
      empty[0] = NULL;
      return empty;
    }
    temp = temp->children[idx];
  }
  size_t capacity = 8, count = 0;
  char **matches = malloc(capacity * sizeof(char *));
  char *buffer = malloc(1024);
  strcpy(buffer, word);
  trie_collect(temp, buffer, len, &matches, &count, &capacity);
  free(buffer);

  matches = realloc(matches, (count + 1) * sizeof(char *));
  matches[count] = NULL;
  return matches;
}
