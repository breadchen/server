#include "commanddata.h"

struct command_ch command_str[MAX_HEAD_LEN][USED_CHAR_NUM];

char* follows0[USED_CHAR_NUM] = 
{
	0,0,0,0,0,0,0,0,0,0,
	0,"E",0,0,0,0,"U",0,0,0,
	0,0,0,0,0,0
};
char* follows1[USED_CHAR_NUM] = 
{
	0,0,0,0,"D",0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	"I",0,0,0,0,0
};
char* follows2[USED_CHAR_NUM] = 
{
	0,0,0,0,0,0,0,0,"T",0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0
};
char* follows3[USED_CHAR_NUM] = 
{
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0
};

char** follows[MAX_HEAD_LEN] =
{
	follows0,follows1,follows2,follows3
};
