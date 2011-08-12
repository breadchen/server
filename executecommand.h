#ifndef _EXECUTE_COMMAND_H
#define _EXECUTE_COMMAND_H

#define QUIT_SUBSERVER -2

extern int execute_command(const char* command, int len);
extern int init_command_data();

#endif // if (!_EXECUTE_COMMAND_H)
