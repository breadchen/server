#include "commanddata.h"

struct command command_list[CMD_NUM];
int command_index[USED_CHAR_NUM];

char* command_head[CMD_NUM] = 
{
	"LED",
	"QUIT"
};
