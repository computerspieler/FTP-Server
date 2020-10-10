#include <stdio.h>
#include <stdlib.h>

#include "ftp.h"
#include "network.h"
#include "typedef.h"

int main(int argc, char* argv[])
{
	int return_val = 0;

	return_val = network_init("127.0.0.1");
	if(return_val)
		return return_val;

	network_run();
	network_exit();

	return 0;
}
