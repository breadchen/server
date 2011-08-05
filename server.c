#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"
#include "errreport.h"
#include "subserver.h"
#include "pidmanager.h"
#include "responsecode.h"

extern int get_local_address(struct sockaddr_in *addr_out);
extern int addr_convert(char* addr, struct in_addr *addr_out); 
extern void child_handler(int);
extern void close_all_subserver(void);
extern void term_handler(int sig);
	
static int n_pid_parent= 0;

int main(int argc, char** argv)
{
	int n_serversock = 0;
	int n_clientsock = 0;
	int n_clientaddr_len = 0;
	int n_pid = 0;
	int n_can_add;
	char charbuf[BUF_SIZE];
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;

	int i = 0;
	
	// init socket
	n_serversock = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > n_serversock)
	{
		PRINT_ERR("init socket error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if
	
	// init address data	
	if (argc >= LEAST_ARG_NUM &&
		FUC_SUCCESS == addr_convert(argv[1], &serv_addr.sin_addr)
		)
	{
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);
	} // end of if
	else
	{ // if can't use arguments, use local address instead
		if (argc >= LEAST_ARG_NUM)
		{
			PRINT_ERR("can not recognize the ip address or hostname")
		} // end of if
		PRINT_ERR("use local address")

		bzero(&serv_addr, sizeof(serv_addr));
		if (FUC_FAILURE == get_local_address(&serv_addr))
		{
			PRINT_ERR("can not get local address")
			exit(EXIT_FAILURE);	
		} // end of if
	} // end of else
	
	// bind socket to address
	if (bind(n_serversock, 
			 (struct sockaddr*)&serv_addr, 
			 sizeof(serv_addr)) < 0)
	{
		PRINT_ERR("bind error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if

	// change socket to listen state
	if (listen(n_serversock, BACK_LOG) < 0)
	{
		PRINT_ERR("listen error")
		PRINT_ERR(strerror(errno))
		exit(EXIT_FAILURE);
	} // end of if
	
	// handle SIGCHLD signal
	if (SIG_ERR == signal(SIGCHLD, &child_handler))
	{
		PRINT_ERR("couldn't registe signal handler")
		exit(EXIT_FAILURE);
	} // end of if
	
	if (SIG_ERR == signal(SIGTERM, &term_handler))
	{
		PRINT_ERR("couldn't registe SIGTERM handler")
		exit(EXIT_FAILURE);
	} // end of if
	
	n_pid_parent = getpid();
	atexit(close_all_subserver);

	init_manager();
	// handle client request
	while (i++ < 6)
	{
		// wait for client
		n_clientsock = accept(n_serversock, 
							  (struct sockaddr*)&client_addr, 
							  &n_clientaddr_len);
		if (n_clientsock < 0)
		{
			PRINT_ERR("accept error")
			PRINT_ERR(strerror(errno))
			continue;
		} // end of if

		n_can_add = can_add();
		// invoke subserver to handle client request
		if ((n_pid = fork()) < 0)
		{
			PRINT_ERR("fork error")
			PRINT_ERR(strerror(errno))
		} // end of if
		else
		{
			if (n_pid == 0)
			{ // child process
				close(n_serversock);

				// if pid manager is full
				if (FUC_FAILURE == n_can_add)
				{
					subserver(n_clientsock, SERVER_BUSY);
					close(n_clientsock);
					exit(EXIT_FAILURE);
				} // end of if

				if (FUC_FAILURE == subserver(n_clientsock, WELCOME))
				{
					close(n_clientsock);
					exit(EXIT_FAILURE);
				} // end of if

				close(n_clientsock);
				exit(EXIT_SUCCESS);
			} // end of if
			else
			{ // parent process
				close(n_clientsock);

				if (FUC_SUCCESS == n_can_add)
				{
					if (0 == kill(n_pid, 0))
					{
						add_pid(n_pid); // add pid to pid manager
						sprintf(test_buf, "adding pid[%d]", n_pid);
						PRINT_TEST(test_buf);
					} // end of if
				} // end of if
			} // end of else
		} // end of else
	} // end of while

	close(n_serversock);
	return EXIT_SUCCESS;
} // end of main


/*
 * Function: get local ip and port,
 * 	success return FUC_SUCCESS, failure return FUC_FAILURE.
 */
int get_local_address(struct sockaddr_in *addr_out)
{
	addr_out->sin_family = AF_INET;
	addr_out->sin_port = htons(PORT);
	addr_out->sin_addr.s_addr = htonl(INADDR_ANY);
////if (0 == inet_aton(IP, &(addr_out->sin_addr)))
////{
////	PRINT_ERR(strerror(errno))
////	return FUC_FAILURE;
////} // end of if
	return FUC_SUCCESS;
} // end of get_local_address()


/*
 * Function: convert the string of ip address or hostname
 * 	to struct inaddr, success return FUC_SUCCESS,
 * 	failure return FUC_FAILURE
 */
int addr_convert(char *addr, struct in_addr *addr_out)
{
	struct hostent *he;

	if (1 == inet_aton(addr, addr_out))
	{
		return FUC_SUCCESS;
	} // end of if

	he = gethostbyname(addr);
	if (NULL != he)
	{
		*addr_out = *((struct in_addr *)(he->h_addr_list[0]));
		return FUC_SUCCESS;
	} // end of if 

	return FUC_FAILURE;
} // end of addr_convert()


/*
 * Function: handle SIGCHLD signal
 */
void child_handler(int sig)
{
	int n_exit_status = 0;
	int pid = 0;
	int pos = -1;

	pid = wait(&n_exit_status);

	// remove pid from pid manager
	if (FUC_FAILURE != (pos = get_pos(pid)))
	{
		rm_pid(pos);
		sprintf(test_buf, "remove pid[%d]", pid);
		PRINT_TEST(test_buf)
	} // end of if
	
	sprintf(test_buf, 
			"child process[%d] exit with code %d",
			pid,
			n_exit_status);
	PRINT_TEST(test_buf)
} // end of child_handler()


/*
 * Function: send SIGUSR1 to all subservers when exit
 */
void close_all_subserver(void)
{
	int i;
	int pid_count;
	int* pids;

	if (n_pid_parent != getpid())
		return;

	if (NULL == (pids = rm_all(&pid_count)))
		return;

	for (i = 0; i < pid_count; i++)
	{
		sprintf(test_buf, "closing subserver[%d]", pids[i]);
		PRINT_TEST(test_buf)
		kill(pids[i], CLOSE_SUBSERVER);
	} // end of for
} // end of close_all_subserver()

/*
 * Function: handle SIGTERM signal
 */
void term_handler(int sig)
{
	close_all_subserver();
	exit(EXIT_SUCCESS);
} // end of term_handler()
