#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

	int yes = 1;
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("setsocket");
		exit(1);
	}

	if (bind(server, (struct sockaddr *)&address, sizeof(address))) {
		perror("bind");
		exit(1);
	}

	if (listen(server, 10) < 0) {
		perror("listen");
		exit(1);
	}
	
	printf("listening on: 8080\n");
	while(1) {
		int client = accept(server, NULL, NULL);
		if (client < 0) {
			perror("accept");
			exit(1);
		}
		printf("client fd: %d\n", client);

		char request[4096];
		ssize_t n = read(client, request, sizeof(request) - 1);
		if (n <= 0) {
			close(client);
			continue;
		}

		request[n] = '\0';
		char method[8], path[256];
		if (sscanf(request, "%7s %255s", method, path) != 2) {
			close(client);
			continue;
		}
		printf("%s %s\n", method, path);

		char filename[512];
		snprintf(filename, sizeof(filename), "www%s",
			strcmp(path, "/") == 0 ? "/index.html" : path);

		char *response = 
			"HTTP/1.0 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"\r\n"
			"<h1>Hello Nyx!</h1>\n";

		write(client, response, strlen(response));
		close(client);
	}

	return 0;
}
