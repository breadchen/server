#include "commanddata.h"
#include <stdlib.h>

struct command command_list[CMD_NUM];
int command_index[USED_CHAR_NUM];

char* command_head[CMD_NUM] = 
{
	"LED",
	"QUIT"
};

char* option_led[OPT_LED_NUM] = 
{
	"ON", "OFF"
};

char** option[CMD_NUM] = 
{
	option_led,
	NULL
};

