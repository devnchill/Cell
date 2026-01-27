#ifndef FLUSH_BUFFER_TO_ARGV
#define FLUSH_BUFFER_TO_ARGV

typedef struct parsed_command pc;
typedef struct parser_state ps;

void flush_buffer_to_argv(ps *state, pc *cmd);

#endif
