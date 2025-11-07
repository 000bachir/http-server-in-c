#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 6969

void tcp_connection() {
  int server_fd, socket_client;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  char server_message[1024], client_message[1024];

  // clean buffer
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // create the socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1) {
    printf("Socket creation failed...\n");
    exit(0);
  } else {
    printf("socket created successfully\n");
  };
  // initiate port and ip
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // losing the socket already in use because if we restart the server the port
  // will be still hanging for a bit so that we do not bump into the case of
  // socket already in use
  int re_use = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &re_use, sizeof(re_use)) <
      0) {
    printf("error SO_REUSEADDR failded ...\n");
    exit(0);
  }

  // binding the socket to the port and ip created
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    printf("failed to bind to port and ip address \n");
    exit(0);
  } else {
    printf("binded successfully to the port and ip address \n");
  }

  // listen to incoming connection
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("error while listening to incoming connections...\n");
    return;
  } else {
    printf("listening to incoming messages\n");
  }

  // accepting incoming connection
  // client_size = sizeof(client_addr);
  // socket_client =
  //     accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
  // if (socket_client == -1) {
  //   printf("error accepting incoming connection from the client... \n");
  // }

  while (1) {
    printf("waiting for incoming connection\n");
    client_size = sizeof(client_addr);
    socket_client =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
    if (socket_client == -1) {
      printf("connection failed to be accepted\n");
      continue;
    }

    printf("client connected form %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));
    close(socket_client);
  }

  // receiving clinet connection
  // if (recv(socket_client, client_message, sizeof(client_message), 0) < 0) {
  //   printf("could not receive client message...\n");
  // } else {
  //   printf("client message received: %s\n", client_message);
  // }
  ssize_t bytes_received =
      recv(socket_client, client_message, sizeof(client_message) - 1, 0);
  if (bytes_received < 0) {
    printf("could not receive the message sent by the client\n");
  } else {
    client_message[bytes_received] = '\0'; // null termenated
    printf("client message received %s\n", client_message);
  }

  // server responding
  strcpy(server_message, "this is the server message");

  if (send(socket_client, server_message, strlen(server_message), 0) < 0) {
    printf("error server in responding...\n");
  } else {
    printf("%s\n", server_message);
  }

  // close the socket

  close(server_fd);
  close(socket_client);
}
