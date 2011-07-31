#include <stdlib.h>

#include "config.h"
#include "errreport.h"

#define CMD_EXECUTED 0

extern int execute_command(char command[]);
extern int on_subserver_quit();

int subserver(int n_clientsock)
{
	char msg_recv[MAX_MSG_LEN];
	char charbuff[BUF_SIZE];
	int n_received;

	sprintf(test_buf, "%d", n_clientsock);
	PRINT_TEST(test_buf)

	while (1)
	{
		if (0 == (n_received = recv(n_clientsock, 
						           msg_recv, 
								   MAX_MSG_LEN, 
								   0)))
		{
			PRINT_ERR(strerror(errno))
			return FUC_FAILURE;
		} // end of if

		if ('q' == msg_recv[0] || 'Q' == msg_recv[0])
		{
			on_subserver_quit();
			break;
		} // end of if

		if (FUC_FAILURE == execute_command(msg_recv))
		{
			sprintf(charbuff, "%d", 1);
		} // end of if
		else
		{
			sprintf(charbuff, "%d", CMD_EXECUTED);
		} // end of else

		send(n_clientsock, charbuff, strlen(charbuff) + 1, 0);
	} // end of while

	return FUC_SUCCESS;
} // end of main()

int execute_command(char command[])
{
	printf("%s \n", command);
	return FUC_SUCCESS;
} // end of execute_command()

int on_subserver_quit()
{
	return FUC_SUCCESS;
} // end of on_subserver_quit()
