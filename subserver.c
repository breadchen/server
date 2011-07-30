#include <stdlib.h>

#include "config.h"
#include "errreport.h"

int subserver(int n_clientsock)
{
	char msg_recv[MAX_MSG_LEN];

	sprintf(test_buf, "%d", n_clientsock);
	PRINT_TEST(test_buf)

	

	return FUC_SUCCESS;
} // end of main()
