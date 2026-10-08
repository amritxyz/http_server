#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
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

	return 0;
}
