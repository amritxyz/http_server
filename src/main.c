#include <stdio.h>
#include <stdlib.h>

#include "server.h"

int main(int argc, char *argv[])
{
	int port = DEFAULT_PORT;

	if (argc > 1) {
		port = atoi(argv[1]);

		if (port <= 0 || port > 65535) {
			fprintf(stderr, "Error: Invalid port number. Must be 1 to 65535\n");
			exit(1);
		}
	}

	start_server(port);

	return 0;
}
