#include "web_server.h"


response *handle_request(request *req) {
	(void)req;
	response *res = (response *)malloc(sizeof(response));
	res->status_code = 200;
	res->status_text = "OK";
	res->content_type = "text/html";
	res->content = "<html><body><h1>Hello, World!</h1></body></html>";
	return res;
}
int main() {
  web_server *server = (web_server *)malloc(sizeof(web_server));
 	int result = create_server(server, *handle_request); 
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
