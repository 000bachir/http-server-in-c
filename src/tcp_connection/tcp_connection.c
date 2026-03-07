#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 6969
#define BUFFER_SIZE 4096

int parse_request_line(const char *buffer, char *path, char *method) {
  return sscanf(buffer, "%15s %255s", method, path) == 2;
}
// send response
void send_response(int client_fd, int status_code, const char *status_text,
                   const char *content_type, const char *body) {
  char header[BUFFER_SIZE];
  int body_len = strlen(body);
  // http requires \r\n line ending and an empty line between headers and body
  snprintf(header, sizeof(header),
           "HTTP 1.1 %d %s\r\n"
           "Content-Type : %s\r\n"
           "Content-Length : %d\r\n"
           "Connection : close\r\n"
           "\r\n", // the empty line needed between the header and the body
           status_code, status_text, content_type, body_len);

  send(client_fd, header, strlen(header), 0);
  send(client_fd, body, body_len, 0);
};

// handeling one client connection :
void handle_client_connection(int client_fd) {
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_received <= 0) {
    if (bytes_received == 0) {
      fprintf(stderr, "Client disconnected before sending data \n");
    } else {
      perror("recv failed");
      return;
    }
  }
  printf("--- Request ---\n%s\n---------------\n", buffer);

  char method[16], path[256];
  if (!parse_request_line(buffer, method, path)) {
    send_response(client_fd, 400, "Bad Request", "text/plain",
                  "400 Bad Request");
  };
  printf("Method : %s  | Path : %s\n", method, path);
  if (strcmp(method, "GET") != 0) {
    send_response(client_fd, 405, "Method not allowed", "text/palin",
                  "405 Method not allowed");
    return;
  }
  if (strcmp(path, "/") == 0) {
    const char *body =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <title>My Server</title>\n"
        "  <style>\n"
        "    body { font-family: Arial, sans-serif; background: #f0f4f8; "
        "    color: #333; text-align: center; padding: 60px 20px; postion : "
        "relative; overflow: hidden ;  }\n"
        "    h1   { color: #2c3e50; margin-bottom: 20px; }\n"
        "    p    { font-size: 1.2em; }\n"
        "  </style>\n"
        "</head>\n"
        "<body>\n"
        "  <h1>Hello from my C HTTP server!</h1>\n"
        "  <p>Running on port 6969</p>\n"
        "</body>\n"
        "</html>";
    send_response(client_fd, 200, "OK", "text/plain", body);
  } else if (strcmp(path, "/hello") == 0) {
    const char *body =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <title>My Server</title>\n"
        "  <style>\n"
        "    body { font-family: Arial, sans-serif; background: #f0f4f8; "
        "    color: #333; text-align: center; padding: 60px 20px; postion : "
        "relative; overflow: hidden ;  }\n"
        "    h1   { color: #2c3e50; margin-bottom: 20px; }\n"
        "    p    { font-size: 1.2em; }\n"
        "  </style>\n"
        "</head>\n"
        "<body>\n"
        "  <h1>Hello from route of my C HTTP server!</h1>\n"
        "  <p>Running on port 6969</p>\n"
        "</body>\n"
        "</html>";

    send_response(client_fd, 200, "ACCEPTED", "text/plain", body);
  } else {
    send_response(
        client_fd, 404, "NOT FOUND", "text/html",
        "<!DOCTYPE html><html><body><h1>404 Not Found</h1></body></html>");
  };
};

void tcp_connection() {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_size = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  // create the socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Socket creation failed...\n");
    exit(EXIT_FAILURE);
  } else {
    printf("socket created successfully\n");
  };

  // Reuse address so restart doesn't fail with "address already in use"
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("set socket failed \n");
    exit(EXIT_FAILURE);
  }

  // Configure address structure
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    perror("invalid address\n");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // binding
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("binding failed\n");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  // listen
  if (listen(server_fd, 10) < 0) {
    perror("server listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Listening on http://127.0.0.1:%d ...\n",
         PORT); // \n ensures it prints
  while (1) {

    client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
    if (client_fd < 0) {
      perror("accept failed");
      continue;
    }

    // printing client address
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    printf("client connected %s\n", client_ip);
    handle_client_connection(client_fd);
    close(client_fd);

    // now i receive data
    // memset(buffer, 0, BUFFER_SIZE);
    // ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    // if (bytes_received > 0) {
    //   printf("received %s\n", buffer);
    //
    //   const char *message = "message received by the server";
    //   if (send(client_fd, message, strlen(message), 0) < 0) {
    //     perror("send failed\n");
    //   }
    // } else if (bytes_received == 0) {
    //   perror("client disconnected");
    // } else {
    //   perror("recv failed");
    // };
  }
  close(server_fd);
}
