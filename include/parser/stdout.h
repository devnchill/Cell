#ifndef PARSE_STDOUT
#define PARSE_STDOUT

typedef struct parsed_command pc;
typedef struct parser_state ps;

void parse_stdout(pc *cmd, ps *state, int inc);

#endif
