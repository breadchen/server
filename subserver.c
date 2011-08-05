#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "config.h"
#include "errreport.h"
#include "message.h"
#include "responsecode.h"

extern int receive_command(int sockfd, char* command_out);
extern int execute_command(char command[]);
extern int on_subserver_quit();
extern void close_subserver(int sig);

static int sockfd;

int subserver(int n_clientsock, int rsp)
{
	char cmd_recv[MAX_MSG_LEN];
	char charbuff[BUF_SIZE];
	int n_received;
	int execute_result;

	//sprintf(test_buf, "%d", n_clientsock);
	//PRINT_TEST(test_buf)
	sockfd = n_clientsock;
	
	sprintf(charbuff, "%d", rsp);
	if (FUC_FAILURE == send_response(n_clientsock, 
									 charbuff,
									 0, 
									 strlen(charbuff) + 1))
	{
		printf("couldn't send response to client\n");
	} // end of if

	if (SERVER_BUSY == rsp)
	{
		return EXIT_SUCCESS;	
	} // end of if

	if (SIG_ERR == signal(CLOSE_SUBSERVER, &close_subserver))
	{
		PRINT_ERR("couldn't registe signal handler")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if

	while (1)
	{
		n_received = receive_command(n_clientsock, cmd_recv);
		if (FUC_FAILURE == n_received)
		{
			return FUC_FAILURE;
		} // end of if

		execute_result = execute_command(cmd_recv);
		if (FUC_FAILURE == execute_result) 
		{
			sprintf(charbuff, "%d", EXEC_FAILURE);
		} // end of if
		else
		{
			if (QUIT_SUBSERVER == execute_result)
			{
				break;
			} // end of if

			sprintf(charbuff, "%d", EXEC_SUCCESS);
		} // end of else
		
		if (FUC_FAILURE == send_response(n_clientsock, 
										 charbuff,
										 0, 
										 strlen(charbuff) + 1))
		{
			printf("couldn't send response to client\n");
		} // end of if
	} // end of while

	on_subserver_quit();
	return FUC_SUCCESS;
} // end of main()


int execute_command(char command[])
{
	if ('q' == command[0] || 'Q' == command[0])
		return QUIT_SUBSERVER;
	printf("client: %s \n", command);
	return FUC_SUCCESS;
} // end of execute_command()


int on_subserver_quit()
{
	sprintf(test_buf, "subserver quit pid[%d]", getpid());
	PRINT_TEST(test_buf)

	return FUC_SUCCESS;
} // end of on_subserver_quit()


int receive_command(int sockfd, char* command_out)
{
	int n_offset = 0;
	int n_counter = 0;
	char buf[sizeof(struct message)];
	struct message* msg;
	
	do
	{
		n_counter = recv(sockfd, 
				         &(buf[n_offset]), 
						 sizeof(struct message) - n_offset, 
						 0);
	
       	if (n_counter > 0)
			n_offset += n_counter; 
		else
		{
			PRINT_ERR("receive error")
			PRINT_ERR(strerror(errno))
			return FUC_FAILURE;
		} // end of if
	} while (n_offset < sizeof(struct message));

	msg = (struct message*)buf;

	strncpy(command_out, msg->cmd, msg->len);

	return msg->len;
} // end of receive_command()


int send_response(int sockfd, char* content, int offset, int len)
{
	struct message msg;

	extern int wrapper(struct message*, char*, int, int);
	extern int _send(int sockfd, const struct message* msg);

	if (FUC_FAILURE == wrapper(&msg, content, offset, len))
	{
		return FUC_FAILURE;
	} // end of if
	
	if (FUC_FAILURE == _send(sockfd, &msg))
	{
		return FUC_FAILURE;
	} // end of if

	return FUC_SUCCESS;
} // end of send_response()


/*
 * Fuction: send struct message through sock, 
 *  make sure all parts of message have been sent
 */
int _send(int sockfd, const struct message* msg)
{
	int n_counter = 0;
	int n_offset = 0;
	do
	{
		n_counter = send(sockfd, 
						 ((char*)msg) + n_offset,
						 sizeof(struct message) - n_offset,
						 0);

		if (n_counter > 0)
			n_offset += n_counter;
		else
		{
			PRINT_ERR("send error")
			PRINT_ERR(strerror(errno))
			return FUC_FAILURE;
		} // end of else
	} while (n_offset < sizeof(struct message));

	return FUC_SUCCESS;
} // end of _send()

/*
 * Function: put string content in struct message
 */
int wrapper(struct message* msg_out, char* content, int offset, int len)
{
	int n_counter = 0;

	if (offset < 0 || len < 0 || offset + len > MAX_MSG_LEN) 
		return FUC_FAILURE;

	strncpy(msg_out->head, MSG_HEAD, MSG_HEAD_LEN);
	msg_out->len = len;
	strncpy(msg_out->cmd, &(content[offset]), len);

	return FUC_SUCCESS;
} // end of wrapper()


/*
 * Function:
 */
void close_subserver(int sig)
{
	char buf[10];

	sprintf(buf, "%d", SERVER_QUIT);
	send_response(sockfd, buf, 0, strlen(buf) + 1);
	sprintf(test_buf, "subserver[%d] closed", getpid()); 
	PRINT_TEST(test_buf)

	exit(EXIT_SUCCESS);
} // end of close_subserver()
