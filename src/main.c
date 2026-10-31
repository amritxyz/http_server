#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

int main(void)
{
	int32_t server = socket(AF_INET, SOCK_STREAM, 0);
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

	uint8_t yes = 1;
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
		int32_t client = accept(server, NULL, NULL);
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

		struct stat info;
		int8_t file = -1;
		if (stat(filename, &info) < 0 || !S_ISREG(info.st_mode) ||
				(file == open(filename, O_RDONLY) < 0)) {

			char *response = 
				"HTTP/1.0 404 not found\r\n"
				"Content-Type: text/html\r\n"
				"\r\n"
				"<h1>404!</h1>\n";
			write(client, response, strlen(response));
			close(client);
			continue;
		}

		char header[256];
		int length = snprintf(header, sizeof(header), 
			"HTTP/1.0 200 OK\r\n"
			"Content-Type: %s\r\n"
			"Content-Length: %ld\r\n"
			"\r\n"
			"placeholder", (long)info.st_size
		);
		write(client, header, length);

		char buffer[4096];
		ssize_t count;

		while((count = read(file, buffer, sizeof(buffer))) > 0) {
			write(client, buffer, count);
		}

		close(file);
		close(client);
	}

	return 0;
}
