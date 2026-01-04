#include "../../include/builtin_echo.h"
#include "../../include/builtin_exit.h"
#include "../../include/builtin_type.h"
#include "../../include/hashmap.h"
#include "../../include/shell_builtin.h"

void add_builtins(void) {
  // exit
  shell_builtin exit;
  exit.command = "exit";
  exit.info = "terminate the program with return code 0";
  exit.func = builtin_exit;
  hashmap_add(exit.command, exit);

  shell_builtin echo;
  echo.command = "echo";
  echo.info = "print everything after echo";
  echo.func = builtin_echo;
  hashmap_add(echo.command, echo);

  shell_builtin type;
  type.command = "type";
  type.info = "check whether the type of command is builtin or not";
  type.func = builtin_type;
  hashmap_add(type.command, type);
}
