#include <stdlib.h>
#include <stdio.h>
#include "server.h"



int main(int argc, char *argv[])
{
	int lsd;

	signal(SIGINT, Sigint_handler);

	int port = atoi(argv[1]);

	lsd = CreateServer(port);

	RunServer(lsd);

	return 0;
}

