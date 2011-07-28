#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "address.h"
#include "errreport.h"

int main(int argc, char** argv)
{
	int n_socketfd = 0;
	
	// init socket
	n_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 == n_socketfd)
	{
		PRINT_ERR(strerror(errno));
		exit(EXIT_FAILURE);
	} // end of if

	// init address data

	return EXIT_SUCCESS;
} // end of main
