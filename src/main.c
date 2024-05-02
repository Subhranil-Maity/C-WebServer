#include "web_server.h"
#include <stdio.h>

response *handle_request(request *req) {
	(void) req;
  response *res = (response *)malloc(sizeof(response));
  res->status_code = 200;
  res->status_text = "OK";
  res->content_type = "text/html";
  res->content = "<html><body><h1>Hello, World!</h1></body></html>";
  // res->content = "<html><body><h1>Hello, World! 100000</h1></body></html>";
  return res;
}
int main() {

	// return 0;
  web_server *server = (web_server *)malloc(sizeof(web_server));
	logger *loger = (logger *)malloc(sizeof(logger));
	loger->debug = true;
	loger->info = true;
	loger->warn = true;
	loger->error = true;
	loger->time = true;
	loger->date = true;
  int result = create_server(server, *handle_request, loger);

  if (result == -1) {
    perror("Server creation failed");
    exit(EXIT_FAILURE);
  }
  if (start_server(server) == -1) {
    perror("Server creation failed");
    exit(EXIT_FAILURE);
  }

  // Close server socket
  close(server->server_socket_fd);
  free(server);
  return 0;
}
