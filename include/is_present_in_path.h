#ifndef EXECUTABLE_PRESENT_IN_SYSTEM_PATH
#define EXECUTABLE_PRESENT_IN_SYSTEM_PATH

#include <stdbool.h>

typedef struct executable_in_system_path {
  bool is_present;
  char full_path[1024];
} executable_in_system_path;

executable_in_system_path executable_is_present(char *executable_name);

#endif
