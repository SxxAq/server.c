#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000

int main() {
  int server_fd;
  struct sockaddr_in address;

  // socket file descriptor initialization
  // representing the server's listening socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // sockaddr_in structure define
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind the socket to specified IP and PORT
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) > 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Start listening for connections
  if (listen(server_fd, 10) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server is listening on PORT: %d...\n", PORT);

  return 0;
}
