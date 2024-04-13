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
  if (start_server(server) == -1) {
    perror("Server creation failed");
    exit(EXIT_FAILURE);
  }
  int client_socket_fd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  printf("Server listening on port %d...\n", PORT);

  // Accept incoming connections and handle them
  while (1) {
    client_socket_fd =
        accept(server->server_socket_fd, (struct sockaddr *)&client_addr,
               &client_addr_len);
    if (client_socket_fd == -1) {
      perror("Accept failed");
      exit(EXIT_FAILURE);
    }

    handle_client(client_socket_fd, *handle_request);
  }

  // Close server socket
  close(server->server_socket_fd);
  free(server);
  return 0;
}
