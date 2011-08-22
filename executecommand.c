#include <stdlib.h>

#include "config.h"
#include "errreport.h"
#include "executecommand.h"
#include "commanddata.h"
#include "responsecode.h"

extern int quit_(char* args, int offset, int len);
extern int led_(char* args, int offset, int len);

int execute_command(const char* command, int len)
{
	char* cmd = (char*)malloc(len);
	int ch_counter;
	int ch_pos = -1;
	int i, j;
	int result;
	int max_len = len < MAX_HEAD_LEN ? len : MAX_HEAD_LEN;
	extern int toupper_(char*, int, int);
	extern int map(char ch);
	extern int ch_in_str(char ch, const char* str);

	if (len <= 0) goto failure;
	strncpy(cmd, command, len);
	
	if (FUC_FAILURE == toupper_(cmd, 0, len))
	{
		PRINT_ERR("command length error")
		goto unrecognized;
	} // end of if

	sprintf(test_buf, "client: %s", command);
	PRINT_TEST(test_buf)

	// mismatch
	ch_counter = 0;
	do
	{
		// if command head can not be recognized return FUC_FAILURE
		if (ch_counter > 0 && ch_pos > -1)
		{
			if (FUC_FAILURE == ch_in_str(
					cmd[ch_counter],
					command_str[ch_counter - 1][ch_pos].follows)
			   )goto unrecognized;
		} // end of if

		// if command head contains chars not in [A-Z] return FUC_FAILURE
		if (FUC_FAILURE == (ch_pos = map(cmd[ch_counter])))
			goto unrecognized;
		ch_counter++;
	} while (' ' != cmd[ch_counter] && ch_counter < max_len);

	// match
	for (i = command_index[map(cmd[0])]; i < CMD_NUM; i++)
	{
		for (j = 0; 
			 j < ch_counter; 
			 j++)
		{
			if (command_list[i][j] != cmd[j])
			{
				if (0 == j) goto unrecognized;
				break;
			} // end of if
		} // end of for

		if (j == ch_counter && 
			(MAX_HEAD_LEN == ch_counter || '\0' == command_list[i][j])
		   )
		{
			if (command_list[i][CMD_LIST_WIDTH - 1] > 0)
			{
				result = ((int (*)(char*, int, int))
							(command_list[i][CMD_LIST_WIDTH - 1])
					     )(cmd, ch_counter, len);
				goto success;
			} // end of if
		} // end of if
	} // end of for

unrecognized:
	free(cmd);
	return CMD_UNRECOGNIZED;
success:
	free(cmd);
	return result;
failure:
	free(cmd);
	return FUC_FAILURE;
} // end of execute_command()


/* 
 * Function: initialize command identification data,
 *  bind command handle function to command head
 */
int init_command_data()
{
	int ch_num;
	int i;
	int last_pos;
	extern int map(char ch);

	for (ch_num = 0; ch_num < MAX_HEAD_LEN; ch_num++)
	{
		for (i = 0; i < USED_CHAR_NUM; i++)
		{
			command_str[ch_num][i].follows = (char*)(&follows[ch_num][i][0]);
			command_str[ch_num][i].doit = NULL;
		} // end of for
	} // end of for

	last_pos = 0;
	for (i = 0; i < USED_CHAR_NUM; i++)
		command_index[i] = -1;
	for (i = 0; i < CMD_NUM; i++)
	{
		if (-1 == command_index[map(command_list[i][0])])
			command_index[map(command_list[i][0])] = i;
	} // end of for

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


int str_split(const char* str, const char separator, char*** result)
{
	int len = strlen(str);
	int i, j;
	int last_separator = -1;
	int cell_num = 0;
	int interval = 0;
	int increase_size = 20;
	int buf_size = 0;

	// initialize buffer
	if (NULL != (*result = (char**)malloc(increase_size * sizeof(int))))
		buf_size = increase_size;
	else
		return FUC_FAILURE;

	for (i = 0; i <= len; i++)
	{
		if (separator == str[i] || '\0' == str[i])
		{
			interval = i - last_separator; // including a '\0'
			if (interval > 1)
			{
				// alloc memory
				if (cell_num >= buf_size)
				{
					if (NULL != (*result = (char**)realloc(*result, (buf_size + increase_size) * sizeof(int))))
						buf_size += increase_size;
					else
						return FUC_FAILURE;
				} // end of if

				(*result)[cell_num] = (char*)malloc(interval * sizeof(char));
				
				// copy substring
				for (j = last_separator + 1; j < i; j++)
					(*result)[cell_num][j - last_separator - 1] = str[j];
				
				(*result)[cell_num][j - last_separator -1] = '\0';
				cell_num++;
			} // end of if
			last_separator = i;
		} // end of if
	} // end of for

	if (cell_num <= 0)
	{
		free(result);
		return FUC_FAILURE;
	} // end of if

	if (NULL != (*result = (char**)realloc(*result, cell_num * sizeof(int))))
		return cell_num;
	else
		return FUC_FAILURE;
} // end of str_split()


int get_opt(char* args, int offset, int len)
{
} // end of get_opt()


int quit_(char* args, int offset, int len)
{
	return QUIT_SUBSERVER;
} // end of quit_()


int led_(char* args, int offset, int len)
{
	int substr_num = 0;
	int i;
	char** substr = NULL;

	if (FUC_FAILURE != (substr_num = str_split(args, ' ', &substr)))
	{
		for (i = 0; i < substr_num; i++)
			PRINT_TEST(substr[i])
	} // end of if

	// free memory
	for (i = 0; i < substr_num; i++)
		free(substr[i]);
	free(substr);
	return EXEC_SUCCESS;
} // end of led_()
