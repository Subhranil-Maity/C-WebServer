
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "web_log.h"
#define PORT 8080
#define BUFFER_SIZE 1024


#define UNABLE_TO_PARSE_REQUEST 1
#define UNABLE_TO_GENERATE_RESPONCE 2
#define SUCCESS 200

#define GET 1
#define POST 2
#define PUT 3
#define DELETE 4

typedef struct response {
  int status_code;
	char *status_text;
	char *content_type;
  char *content;
} response;
typedef struct request {
	int method;
  char *path;
  char *body;
	int body_len;
	char *content_type;
} request;
typedef struct route {
  char *path;
  void (*response_genetor)(request *, response *);
} route;
typedef struct web_server {
  char *ip;
  int port;
  // route **routes;
  int server_socket_fd;
  struct sockaddr_in server_addr;
	response *(*route_handler)(request*);
	logger *log;
} web_server;

/**
 * Creates a server object
 * returns server file discriptor
 */
int create_server(web_server *server, response *(*route_handler)(request*), logger *loger);
/**
 * binds the server to the socket and port
 */
int bind_server(web_server *server);
/**
 * Starts the server and listens for incoming connections
 */
int start_server(web_server *server);

/* handle_client function is a callback function that is called when a client
 * connects to the server. It takes the client socket file descriptor and a
 * route_handler function pointer as arguments. The route_handler function
 * pointer is a function that takes a structured request pointer as an argument
 * and returns a structured response pointer.
 */
void handle_client(int client_socket_fd, web_server *server);

/**
 * Parses the request string and returns a structured request pointer
 */
int parse_request(char *req_buffer, request *req);
/**
 * Generates a response string from a structured response pointer
 */
// int generate_response(response *res, char *res_buffer);

int generate_response(int client_socket_fd, response *res);

void return_result(int err, int client_socket_fd);
