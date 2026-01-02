#include <stdio.h>
#include <string.h>

void get_command(char *command, size_t size) {
  fgets(command, size, stdin);
  command[strcspn(command, "\n")] = '\0';
  return;
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char command[1024];

  while (1) {

    printf("$ ");

    get_command(command, sizeof(command));

    printf("%s: command not found\n", command);
  }

  return 0;
}
