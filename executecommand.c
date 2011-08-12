#include <stdlib.h>

#include "config.h"
#include "errreport.h"
#include "executecommand.h"
#include "commanddata.h"
#include "responsecode.h"

#define CMD_NUM 2
#define LED 0
#define QUIT 1

static int command_list[CMD_NUM][CMD_LIST_WIDTH] = 
{
	{'L', 'E', 'D', '\0', -1},
	{'Q', 'U', 'I', 'T' , -1}
}; // end of command_list[][]

extern int quit_(char* args, int offset, int len);
extern int led_(char* args, int offset, int len);

int execute_command(const char* command, int len)
{
	char* cmd = (char*)malloc(len);
	int ch_counter;
	int ch_pos = -1;
	int i, j;
	int max_len = len < MAX_HEAD_LEN ? len : MAX_HEAD_LEN;
	extern int toupper_(char*, int, int);
	extern int map(char ch);
	extern int ch_in_str(char ch, const char* str);

	strncpy(cmd, command, len);
	if (FUC_FAILURE == toupper_(cmd, 0, len))
	{
		PRINT_ERR("command length error")
		return CMD_UNRECOGNIZED;
	} // end of if

	sprintf(test_buf, "client: %s", command);
	PRINT_TEST(test_buf)

	// incomplete match
	for (ch_counter = 0; 
		 ' ' != cmd[ch_counter] && ch_counter < max_len;
		 ch_counter++)
	{
		// if command head can not be recognized return FUC_FAILURE
		if (ch_counter > 0 && ch_pos > -1)
		{
			if (FUC_FAILURE == ch_in_str(
					cmd[ch_counter],
					command_str[ch_counter - 1][ch_pos].follows)
			   )return CMD_UNRECOGNIZED;
		} // end of if

		// if command head contains chars not in [A-Z] return FUC_FAILURE
		if (FUC_FAILURE == (ch_pos = map(cmd[ch_counter])))
			return CMD_UNRECOGNIZED;

		// recognized command
		if (NULL != command_str[ch_counter][ch_pos].doit)
		{
			return command_str[ch_counter][ch_pos].doit(cmd, ch_counter, len);	
		} // end of if
	} // end of for

	// complete match
	for (i = 0; i < CMD_NUM; i++)
	{
		for (j = 0; 
			 j < ch_counter && command_list[i][j] == cmd[j]; 
			 j++);

		if (j == ch_counter && 
			(MAX_HEAD_LEN == ch_counter || '\0' == command_list[i][j])
		   )
		{
			if (command_list[i][CMD_LIST_WIDTH - 1] > 0)
				return ((int (*)(char*, int, int))
							(command_list[i][CMD_LIST_WIDTH - 1])
					   )(cmd, ch_counter, len);
		} // end of if
	} // end of for

	return CMD_UNRECOGNIZED;
} // end of execute_command()


/* 
 * Function: initialize command identification data,
 *  bind command handle function to command head
 */
int init_command_data()
{
	int ch_num;
	int i;
	extern int map(char ch);

	for (ch_num = 0; ch_num < MAX_HEAD_LEN; ch_num++)
	{
		for (i = 0; i < USED_CHAR_NUM; i++)
		{
			command_str[ch_num][i].follows = (char*)(&follows[ch_num][i][0]);
			command_str[ch_num][i].doit = NULL;
		} // end of for
	} // end of for

	command_str[0][map('Q')].doit = quit_;
	command_str[1][map('U')].doit = quit_;
	command_str[2][map('I')].doit = quit_;
	command_str[3][map('T')].doit = quit_;
    command_str[0][map('L')].doit = led_;
    command_str[1][map('E')].doit = led_;
    command_str[2][map('D')].doit = led_;

	command_list[QUIT][CMD_LIST_WIDTH - 1] = (int)(&quit_);
	command_list[LED][CMD_LIST_WIDTH - 1] = (int)(&led_);
} // end of init_command_data()


/* 
 * Function: map a char value to it's position in array "follows"
 */
int map(char ch)
{
	if (ch > 'Z' || ch < 'A')
		return FUC_FAILURE;
	else
		return ch - 'A';
} // end of map


/*
 * Function: switch [a-z,A-Z] to [A-Z]
 */
int toupper_(char* str, int offset, int len)
{
	int counter;

	if (offset < 0 || len < 0)
		return FUC_FAILURE;

	for (counter = offset; counter - offset < len; counter++)
	{
		if (str[counter] == '\0') return counter - offset;

		if (str[counter] >= 'a' && str[counter] <= 'z')
		{
			str[counter] -= ('a' - 'A');
		} // end of if
		else if (str[counter] > 'A' && str[counter] < 'Z')
		{
			continue;
		} // end of else if
		else 
		{
			continue;
		} // end of else
	} // end of for

	return counter - len;
} // end of toupper_


int ch_in_str(char ch, const char* str)
{
	int i;

	if (str <= 0) return FUC_FAILURE;
	
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == ch)
			return FUC_SUCCESS;
	} // end of for

	return FUC_FAILURE;
} // end of ch_in_str()


int quit_(char* args, int offset, int len)
{
	return QUIT_SUBSERVER;
} // end of quit_()


int led_(char* args, int offset, int len)
{
	return EXEC_SUCCESS;
} // end of led_()
