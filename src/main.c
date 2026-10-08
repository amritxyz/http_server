#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "main.h"

int main(void)
{
	i32 server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		perror("socket");
		exit(1);
	}
	printf("socket fd: %d\n", server);

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	if (bind(server, (struct sockaddr *)&address, sizeof(address))) {
		perror("bind");
		exit(1);
	}
	if (listen(server, 10) < 0) {
		perror("listen");
		exit(1);
	}

	return 0;
}
