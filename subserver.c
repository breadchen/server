#include <stdlib.h>

#include "config.h"
#include "errreport.h"

int main(int argc, char** argv)
{
	sprintf(test_buf, "%s, %s", argv[0], argv[1]);
	PRINT_TEST(test_buf)

	

	return EXIT_SUCCESS;
} // end of main()
