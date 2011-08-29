#ifndef _COMMAND_DATA_H
#define _COMMAND_DATA_H

// max command head length
#define MAX_HEAD_LEN 4
// number of chars used in command head
#define USED_CHAR_NUM 26

#define SEPARATOR ' '

// cmd
#define CMD_NUM 2
#define LED 0
#define QUIT 1

// opt
#define OPT_LED_NUM 2

struct command
{
	char* head;
	int (*doit)(char** argv, int argc);
};

extern int command_index[USED_CHAR_NUM];
extern struct command command_list[CMD_NUM]; 
extern char* command_head[CMD_NUM];
extern char** option[CMD_NUM];

#endif // if (!_COMMAND_DATA_H)
