#include <stdlib.h>

#include "config.h"
#include "errreport.h"
#include "executecommand.h"
#include "commanddata.h"
#include "responsecode.h"

extern int quit_(char** argv, int argc);
extern int led_(char** argv, int argc);

int execute_command(const char* command, int len)
{
	char* cmd = (char*)malloc(len);
	int i;
	int result;
	int start_index;
	int cmd_counter;
	int args_num = 0;
	char** cmd_args = NULL;

	extern int toupper_(char*, int, int);
	extern int map(char ch);
	extern int str_split(const char* str, const char separator, char*** result);

	if (len <= 0) goto failure;
	strncpy(cmd, command, len);
	
	toupper_(cmd, 0, len);

	sprintf(test_buf, "client: %s", command);
	PRINT_TEST(test_buf)

	if (FUC_FAILURE == (args_num = str_split(cmd, SEPARATOR, &cmd_args)))
		goto failure;

	start_index = map(cmd_args[0][0]);
	if (strlen(cmd_args[0]) > MAX_HEAD_LEN || // cmd head too long
		FUC_FAILURE == start_index || // map frist char fail
		-1 == command_index[start_index] // no command head has a char like this at frist position
	   ) goto unrecognized;
	
	// match
	for (cmd_counter = command_index[start_index]; cmd_counter < CMD_NUM; cmd_counter++)
	{
		if (cmd_args[0][0] != command_list[cmd_counter].head[0])
			goto unrecognized;

		if (0 == strcmp(cmd_args[0], command_list[cmd_counter].head))
		{
			if (command_list[cmd_counter].doit != NULL)
			{
				result = ((int (*)(char**, int))
							(command_list[cmd_counter].doit)
					     )(cmd_args, args_num);
				goto success;
			} // end of if
		} // end of if
	} // end of for

unrecognized:
	// free memory
	free(cmd);
	for (i = 0; i < args_num; i++)
		free(cmd_args[i]);
	free(cmd_args);
	return CMD_UNRECOGNIZED;
success:
	// free memory
	free(cmd);
	for (i = 0; i < args_num; i++)
		free(cmd_args[i]);
	free(cmd_args);
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
	int cmd_counter;
	int i;
	extern int map(char ch);

	for (cmd_counter = 0; cmd_counter < CMD_NUM; cmd_counter++)
	{
		command_list[cmd_counter].head = command_head[cmd_counter];
		command_list[cmd_counter].doit = NULL;
	} // end of for

	for (i = 0; i < USED_CHAR_NUM; i++)
		command_index[i] = -1;
	for (i = 0; i < CMD_NUM; i++)
	{
		if (-1 == command_index[map(command_list[i].head[0])])
			command_index[map(command_list[i].head[0])] = i;
	} // end of for

	command_list[QUIT].doit = quit_;
	command_list[LED].doit = led_;
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

				if ('\0' == str[i]) interval--;
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


int get_opt(char** argv, int argc)
{

} // end of get_opt()


int quit_(char** argv, int argc)
{
	return QUIT_SUBSERVER;
} // end of quit_()


int led_(char** argv, int argc)
{
	int i, j;
	int match;
	int counter = 0;
	char output[1024];

	output[counter] = '\0';
	// get options
	for (i = 1; i < argc; i++)
	{
		if ('-' == argv[i][0])
		{
			match = 0;
			for (j = 0; j < OPT_LED_NUM; j++)
			{
				if (0 == strcmp(&(argv[i][1]), option[LED][j]))
				{
					strcat(output, option[LED][j]);
					counter += strlen(option[LED][j]);
					output[counter++] = ':';
					output[counter++] = ' ';
					output[counter] = '\0';
					
					match = 1;
				}
			} // end of for
			if (1 != match) return CMD_UNRECOGNIZED;
		} // end of if
		else
		{
			strcat(output, argv[i]);
			counter += strlen(argv[i]);
			output[counter++] = ' ';
			output[counter] = '\0';
		} // end of else
	} // end of for
	PRINT_TEST(output)

	return EXEC_SUCCESS;
} // end of led_()
