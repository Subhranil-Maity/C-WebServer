#include "web_server.h"
#include <stdbool.h>
#include <string.h>

void log_client(int client_socket_fd, web_server *server) {
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  // Get socket information
  if (getpeername(client_socket_fd, (struct sockaddr *)&addr, &addr_len) == -1) {
    error(server->log, "Get Pere Name");
    return;
  }

  // Extract IP address and port
  int port = ntohs(addr.sin_port);
	char ip[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
	info(server->log, "Connection from %s:%d", ip, port);
}

void handle_client(int client_socket_fd, web_server *server) {
  // char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
  //                   "<html><body><h1>Hello, World!</h1></body></html>";
  log_client(client_socket_fd, server);
  int result = 0;
  char request_buffer[BUFFER_SIZE];
  result = recv(client_socket_fd, &request_buffer, BUFFER_SIZE, 0);
  if (result == -1) {
    perror("Recieve failed");
    return;
  }
  // printf("Request: %s\n", request_buffer);
  request *req = (request *)malloc(sizeof(request));
  result = 0;
  result = parse_request(request_buffer, req);
  if (result == UNABLE_TO_PARSE_REQUEST) {
    fprintf(stderr, "Request parsing failed:\n %s \n", request_buffer);
    return_result(UNABLE_TO_PARSE_REQUEST, client_socket_fd);
    close(client_socket_fd);
    return;
  }
  result = 0;
  response *res = server->route_handler(req);

  result = generate_response(client_socket_fd, res);
  if (result == UNABLE_TO_GENERATE_RESPONCE) {
    fprintf(stderr, "Responce generation failed");
    return_result(UNABLE_TO_GENERATE_RESPONCE, client_socket_fd);
    close(client_socket_fd);
    return;
  }
  free(req);
  free(res);
  // free(&response_text);
  close(client_socket_fd);
}

void return_result(int err, int client_socket_fd) {
  printf("TODO: custom returns\n");
  char *response_text;
  if (err == UNABLE_TO_PARSE_REQUEST) {
    response_text =
        "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
        "<html><body><h1>400 Bad Request</h1></body></html>";
  } else if (err == UNABLE_TO_GENERATE_RESPONCE) {
    response_text =
        "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
        "<html><body><h1>500 Internal Server Error</h1></body></html>";
  }
  send(client_socket_fd, response_text, strlen(response_text), 0);
  close(client_socket_fd);
}
char *intToString(int num) {
  // Determine the number of digits
  int temp = num;
  int digitCount = 0;
  while (temp != 0) {
    temp /= 10;
    digitCount++;
  }

  // Allocate memory for the string
  char *str = (char *)malloc((digitCount + 1) * sizeof(char));

  // Convert each digit to character and store in the string
  for (int i = digitCount - 1; i >= 0; i--) {
    str[i] = num % 10 + '0';
    num /= 10;
  }
  str[digitCount] = '\0'; // Null-terminate the string
  return str;
}
#define CRLF "\r\n"
int generate_response(int client_socket_fd, response *res) {
  (void)res;
  // char response_text[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
  //                    "<html><body><h1>Hello, World!</h1></body></html>";
  char http_ver[] = "HTTP/1.1";
  char con_typ_txt[] = "Content-Type: ";
  int total_len = strlen(http_ver) + 1 + sizeof(res->status_code) + 1 +
                  sizeof(res->status_text) + 1 + sizeof(con_typ_txt) +
                  sizeof(res->content_type) + 2 + sizeof(res->content) + 1;
  char *response_text = malloc(total_len);
  memset(response_text, 0, total_len);
  response_text = strcat(response_text, http_ver);
  response_text = strcat(response_text, " ");
  response_text = strcat(response_text, intToString(res->status_code));
  response_text = strcat(response_text, CRLF);
  response_text = strcat(response_text, con_typ_txt);
  response_text = strcat(response_text, res->content_type);
  response_text = strcat(response_text, CRLF);
  response_text = strcat(response_text, CRLF);
  response_text = strcat(response_text, res->content);
  // r = strcat(r, " ");
  // r = strcat(r, res->status_text);
  // r = strcat(r, "\r\nContent-Type: ");
  // r = strcat(r, res->content_type);
  // r = strcat(r, "\r\n\r\n");
  // r = strcat(r, res->content);

  // printf("%s", c);
  int result = 0;
  result = send(client_socket_fd, response_text, strlen(response_text), 0);
  free(response_text);
  if (result == -1) {
    perror("Send failed");
    return UNABLE_TO_GENERATE_RESPONCE;
  }
  return SUCCESS;
}
int create_server(web_server *server, response *(*route_handler)(request *),
                  logger *loger) {

  // Create socket
  server->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server->server_socket_fd == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }
  server->route_handler = route_handler;
  // Set server address
  server->server_addr.sin_family = AF_INET;
  server->server_addr.sin_addr.s_addr = INADDR_ANY;
  server->server_addr.sin_port = htons(PORT);
  int opt = 1;
  if (setsockopt(server->server_socket_fd, SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  if (loger == NULL) {
    server->log = (logger *)malloc(sizeof(logger));
    server->log->debug = false;
    server->log->info = false;
    server->log->warn = false;
    server->log->error = false;
    server->log->time = false;
    server->log->date = false;
  } else {
    server->log = loger;
  }
  return 0;
}
int bind_server(web_server *server) {
  // Bind socket to address and port
  if (bind(server->server_socket_fd, (struct sockaddr *)&(server->server_addr),
           sizeof(server->server_addr)) == -1) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}
int start_server(web_server *server) {

  // int result = create_server(server, route_handler);
  int result = 0;

  result = bind_server(server);
  if (result == -1) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server->server_socket_fd, 5) == -1) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  int client_socket_fd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  // int f = 1;
  // debug(server->log, , ...)
  info(server->log, "Server listening on port %d...\n", PORT);

  // Accept incoming connections and handle them
  while (1) {
    client_socket_fd =
        accept(server->server_socket_fd, (struct sockaddr *)&client_addr,
               &client_addr_len);
    if (client_socket_fd == -1) {
      perror("Accept failed");
      exit(EXIT_FAILURE);
    }

    handle_client(client_socket_fd, server);
  }
  return 0;
}

int parse_request(char *req_buffer, request *req) {
    // Parse method
    if (strncmp(req_buffer, "GET", 3) == 0) {
        req->method = GET;
    } else if (strncmp(req_buffer, "POST", 4) == 0) {
        req->method = POST;
    } else if (strncmp(req_buffer, "PUT", 3) == 0) {
        req->method = PUT;
    } else if (strncmp(req_buffer, "DELETE", 6) == 0) {
        req->method = DELETE;
    } else {
        return ERROR; // Unknown method
    }

    // Parse path
    char *path_start = strchr(req_buffer, ' ') + 1; // Find the space after method
    char *path_end = strchr(path_start, ' '); // Find the space after path
    int path_len = path_end - path_start;
    req->path = (char *)malloc(path_len + 1);
    if (req->path == NULL) {
        return ERROR; // Memory allocation failed
    }
    strncpy(req->path, path_start, path_len);
    req->path[path_len] = '\0';

    // Parse body if it exists (assuming Content-Length header is present)
    char *content_length_str = strstr(req_buffer, "Content-Length:");
    if (content_length_str != NULL) {
        int content_length;
        if (sscanf(content_length_str, "Content-Length: %d", &content_length) == 1) {
            char *body_start = strstr(req_buffer, "\r\n\r\n") + 4; // Find the end of headers
            req->body = (char *)malloc(content_length + 1);
            if (req->body == NULL) {
                free(req->path);
                return ERROR; // Memory allocation failed
            }
            strncpy(req->body, body_start, content_length);
            req->body[content_length] = '\0';
            req->body_len = content_length;
        } else {
            return ERROR; // Failed to parse Content-Length
        }
    } else {
        req->body = NULL; // No body
        req->body_len = 0;
    }

    // Parse Content-Type if it exists
    char *content_type_str = strstr(req_buffer, "Content-Type:");
    if (content_type_str != NULL) {
        content_type_str += strlen("Content-Type:");
        while (*content_type_str == ' ') {
            content_type_str++;
        }
        char *content_type_end = strchr(content_type_str, '\r');
        if (content_type_end != NULL) {
            int content_type_len = content_type_end - content_type_str;
            req->content_type = (char *)malloc(content_type_len + 1);
            if (req->content_type == NULL) {
                free(req->path);
                free(req->body);
                return ERROR; // Memory allocation failed
            }
            strncpy(req->content_type, content_type_str, content_type_len);
            req->content_type[content_type_len] = '\0';
        }
    }

    return SUCCESS;
}
