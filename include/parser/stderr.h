#ifndef PARSE_STDERR
#define PARSE_STDERR

typedef struct parsed_command pc;
typedef struct parser_state ps;

void parse_stderr(pc *cmd, ps *state);

#endif
