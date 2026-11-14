#ifndef SERVER_H
#define SERVER_H

enum {
	DEFAULT_PORT = 8080
};

// vars
static const char WEB_ASSETS[] = "web";

// functions
const char *content_type(const char *filename);
void start_server(unsigned int port);

#endif
