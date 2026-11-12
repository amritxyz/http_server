#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

const char *
content_type(const char *filename)
{
	const char *dot = strrchr(filename, '.');

	/* INFO:
	 * Unknown file type default to application/octet-stream
	 * Ref: https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/MIME_types/Common_types
	 */
	if (dot == NULL) return "application/octet-stream";

	if (strcmp(dot, ".html") == 0) return "text/html";
	if (strcmp(dot, ".css") == 0) return "text/css";
	if (strcmp(dot, ".js") == 0) return "text/javascript";
	if (strcmp(dot, ".jpg") == 0) return "image/jpeg";
	if (strcmp(dot, ".webp") == 0) return "image/webp";
	if (strcmp(dot, ".json") == 0) return "application/json";
	if (strcmp(dot, ".svg") == 0) return "image/svg+xml";
	if (strcmp(dot, ".pdf") == 0) return "application/pdf";

	return "text/plain";
}

int main(void)
{
	int server_fd;
	struct sockaddr_in server_addr;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		exit(1);
	}
	printf("socket fd: %d\n", server_fd);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	/* Lets us restart without waiting for TIME_WAIT */
	int yes = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	if (bind(server_fd, (struct sockaddr *)&server_addr,
	    sizeof(server_addr)) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(server_fd, 10) < 0) {
		perror("listen");
		exit(1);
	}
	
	printf("listening on: 8080\n");
	while(1) {
		int client = accept(server_fd, NULL, NULL);
		if (client < 0) {
			perror("accept");
			exit(1);
		}
		printf("client fd: %d\n", client);

		/* read the request first, closing with unread data causes an RST*/
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
		if (strstr(path, "..") != NULL) {
			char *response =
				"HTTP/1.0 404 not found\r\n"
				"Content-Type: text/html\r\n"
				"\r\n"
				"<h1>Haha hacker 404!</h1>\n";
			write(client, response, strlen(response));
			close(client);
			continue;
		}

		char filename[512];
		snprintf(filename, sizeof(filename), "web%s",
			strcmp(path, "/") == 0 ? "/index.html" : path);

		struct stat info;

		int file = -1;
		if (stat(filename, &info) == 0 && S_ISREG(info.st_mode))
			file = open(filename, O_RDONLY);
		if (file < 0) {
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
			"\r\n",
			content_type(filename), (long)info.st_size
		);
		write(client, header, (size_t)length);

		char buffer[4096];
		ssize_t count;

		while((count = read(file, buffer, sizeof(buffer))) > 0) {
			write(client, buffer, (size_t)count);
		}

		close(file);
		close(client);
	}

	return 0;
}
