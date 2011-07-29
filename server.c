#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "config.h"
#include "errreport.h"

extern int get_local_address(struct sockaddr_in *addr_out);
extern int addr_convert(char* addr, struct in_addr *addr_out); 

int main(int argc, char** argv)
{
	int n_serversock = 0;
	int n_clientsock = 0;
	int n_clientaddr_len = 0;
	int n_pid = 0;
	int n_exit_status = 0;
	int pid_clients[MAX_CLIENT_NUM];
	int n_client_count = 0;
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
	
	memset(pid_clients, -1, MAX_CLIENT_NUM);
	// handle client request
	while (i++ < 3)
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

		// invoke subserver to handle client request
		if ((n_pid = fork()) < 0)
		{
			PRINT_ERR("fork error")
			PRINT_ERR(strerror(errno))
		} // end of if
		else
		{
			if (n_pid == 0)
			{
				// convert int to char[]
				sprintf(charbuf, "%d", n_clientsock);
				if (execl(SUBSERVER_PATH, 
					 	  SUBSERVER_NAME, 
						  charbuf,
						  NULL) < 0)
				{
					PRINT_ERR("execl error")
					PRINT_ERR(strerror(errno))
					exit(EXIT_FAILURE);
				} // end of if
			} // end of if
			else
			{
				wait(&n_exit_status);
				sprintf(test_buf, 
						"child process exit with code %d\n",
						n_exit_status);
				PRINT_TEST(test_buf)
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
