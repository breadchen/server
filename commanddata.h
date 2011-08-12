#ifndef _COMMAND_DATA_H
#define _COMMAND_DATA_H

// max command head length
#define MAX_HEAD_LEN 4
#define CMD_LIST_WIDTH 5
// number of chars used in command head
#define USED_CHAR_NUM 26

struct command_ch
{
	char* follows;
	int (*doit)(char* args, int offset, int len);
};

extern struct command_ch command_str[MAX_HEAD_LEN][USED_CHAR_NUM];
//extern char* follows0[USED_CHAR_NUM];
//extern char* follows1[USED_CHAR_NUM];
//extern char* follows2[USED_CHAR_NUM];
//extern char* follows3[USED_CHAR_NUM];
extern char** follows[MAX_HEAD_LEN];

//#define GET_FOLLOWS(a,b) follows##a[b]

#endif // if (!_COMMAND_DATA_H)
