#ifndef REDIRECT_STDERR
#define REDIRECT_STDERR

typedef struct parsed_command pc;

void redirect_stderr(pc *command, int *saved_stdout_fd);

#endif
