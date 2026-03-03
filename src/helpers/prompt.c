#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *build_prompt(void) {
  char *cwd = getcwd(NULL, 0);
  if (!cwd)
    return NULL;

  size_t len = strlen(cwd) + 4;
  char *prompt = malloc(len);
  if (!prompt) {
    free(cwd);
    return NULL;
  }

  snprintf(prompt, len, "%s $ ", cwd);
  free(cwd);
  return prompt;
}
