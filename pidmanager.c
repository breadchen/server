#include <stdlib.h>

#include "config.h"
#include "errreport.h"

#define EMPTY -1
#define INCREASE_SIZE 50

static int pids[MAX_CLIENT_NUM];
static int n_pid_count;

/*
 * Function: initialize(clear) pid manager
 */
void init_manager()
{
	memset(pids, EMPTY, sizeof(pids));
	n_pid_count = 0;
} // end of init_manager()


/*
 * Function: test whether array pids is full
 */
int can_add()
{
	extern void gc();

	if (n_pid_count < MAX_CLIENT_NUM)
	{
		if (FUC_FAILURE == find_position(EMPTY))
		{
			//gc();
			//if (FUC_FAILURE == find_position(EMPTY))
				return FUC_FAILURE;
		} // end of if		
		return FUC_SUCCESS;
	} // end of if
	return FUC_FAILURE;	
} // end of can_add()


/*
 * Fuction: add a pid to array pids, 
 *  if array pids is full return FUC_FAILURE
 */
int add_pid(int pid)
{
	int pos;
	extern int find_position(int);
	extern void gc();

	// if pid is already in array pids
	if (FUC_FAILURE != find_position(pid)) return FUC_SUCCESS;

	if (n_pid_count < MAX_CLIENT_NUM)
	{
		if (FUC_FAILURE == (pos = find_position(EMPTY)))
		{
			//gc();
			//if (FUC_FAILURE == (pos = find_position(EMPTY)))
				return FUC_FAILURE;
		} // end of if		

		pids[pos] = pid;
		n_pid_count++;
		return FUC_SUCCESS;
	} // end of if

	return FUC_FAILURE;
} // end of add_pid()


/*
 * Fuction: remove pid at pos, 
 *  success return the pid removed,
 *  fail return FUC_FAILURE
 */
int rm_pid(int pos)
{
	int pid;

	if (pos < 0 || 
		pos >= MAX_CLIENT_NUM)
		return FUC_FAILURE;

	if (EMPTY == pids[pos])
		return FUC_FAILURE;

	pid = pids[pos];
	n_pid_count--;
	pids[pos] = EMPTY;

	return pid;
} // end of rm_pid()


/*
 * Function: remove all pid, 
 *  return an array with pids removed
 */
int* rm_all(int* counter)
{
	int i;
	int pid;
	int buf_size = 0;
	int* result;

	if (0 == n_pid_count)
		return NULL;
	else
	{
		result = (int*)malloc(INCREASE_SIZE * sizeof(int));
		buf_size = INCREASE_SIZE;
	} // end of else

	for (i = 0, *counter = 0;
		 *counter < n_pid_count && i < MAX_CLIENT_NUM;
		 i++)
	{
		if (EMPTY != (pid = pids[i]))
		{
			if (*counter >= buf_size)
			{
				result = (int*)
					realloc(result, (buf_size + INCREASE_SIZE) * sizeof(int));
				if (NULL == result)
				{
					PRINT_ERR("not enough memory")
					return NULL;
				} // end of if
				buf_size += INCREASE_SIZE;
			} // end of if
			
			result[*counter] = pid;
			pids[i] = EMPTY;
			(*counter)++;
		} // end of if
	} // end of for

	result = (int*)realloc(result, (*counter) * sizeof(int));
	return result;
} // end of rm_all()


/*
 * Fuction: find the position of pid in array pids
 */
int get_pos(int pid)
{
	if (pid < 0) return FUC_FAILURE;

	return find_position(pid);
} // end of get_pos()


/* 
 * Fuction: find the position of content in array pids
 */
int find_position(int content)
{
	int counter;

	for (counter = 0; counter < MAX_CLIENT_NUM; counter++)
	{
		if (content == pids[counter]) break;
	} // end of for

	if (counter < MAX_CLIENT_NUM)
		return counter;
	else
		return FUC_FAILURE;
} // end of find_position()


/*
 * Fuction: garbage collector
 */
void gc()
{
	int counter;

	for (counter = 0; counter < MAX_CLIENT_NUM; counter++)
	{
		if (0 != kill(pids[counter], 0))
		{
			rm_pid(counter);
		} // end of if
	} // end of for
} // end of gc()
