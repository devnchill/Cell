#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_if_type_exists(char *input) {
  input = input + 5;
  if (strncmp(input, "echo", 4) == 0) {
    printf("echo is a shell builtin\n");
    return 0;
  }
  if (strncmp(input, "exit", 4) == 0) {
    printf("type is a shell builtin\n");
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Wait for user input
  char input[100];
  while (1) {
    printf("$ ");
    //**********************************************
    // extract input properly and remove new line character
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';
    //************************** handle exit 0 command ***************
    if (strcmp(input, "exit 0") == 0) {
      return 0;
    }
    //****************handle echo command****************************
    if (strncmp(input, "echo", 4) == 0) {
      printf("%s\n", input + 5);
      continue;
    }
    //***************handle type command ***************************
    if (check_if_type_exists(input) == 0) {
      continue;
    }
    printf("%s: command not found\n", input);
  }
  return 0;
}
