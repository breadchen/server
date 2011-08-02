#include <stdlib.h>

#include "config.h"
#include "errreport.h"

#define EMPTY -1

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
 * Fuction: add a pid to array pids, 
 *  if array pids is full return FUC_FAILURE
 */
int add_pid(int pid)
{
	int pos;
	extern int find_position(int);

	// if pid is already in array pids
	if (FUC_FAILURE != find_position(pid)) return FUC_SUCCESS;

	if (n_pid_count < MAX_CLIENT_NUM)
	{
		if (FUC_FAILURE == (pos = find_position(EMPTY)))
		{
			gc();
			if (FUC_FAILURE == (pos = find_position(EMPTY)))
				return FUC_FAILURE;
		} // end of if		

		pids[pos] = pid;
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

	if (n_pid_count == 0 || 
		pos < 0 || 
		pos >= MAX_CLIENT_NUM)
		return FUN_FAILURE;

	pid = pids[pos];
	n_pid_count--;
	pids[pos] = EMPTY;

	return pid;
} // end of rm_pid()


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
	
} // end of gc()
