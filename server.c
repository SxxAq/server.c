#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000

int main() {
  signal(SIGCHLD, SIG_IGN);

  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  int client_fd;
  char buffer[4096] = {0};

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

  while (1) {
    // Accept a connection
    client_fd =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd < 0) {
      perror("accept failed");
      continue;
    }
    // create a child process to handle the client
    if (fork() == 0) {
      close(server_fd); // child doesnt need the listener socket

      printf("Client connected!\n");

      // Read the request into the buffer
      int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
      if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Recieved request:\n%s\n", buffer);

        // const char *response = "HTTP/1.1 200 OK\r\n"
        //                        "Content-Type: text/html\r\n"
        //                        "Content-Length: 13\r\n"
        //                        "\r\n"
        //                        "Hello, world!";
        //

        FILE *file = fopen("index.html", "r");
        if (file == NULL) {
          perror("File not found!");
          close(client_fd);
          continue; // Move to the next request
        }

        // Read file content into buffer
        char file_content[8192]; // assuming HTML fits in this size
        size_t bytes = fread(file_content, 1, sizeof(file_content), file);
        fclose(file);

        // Build full HTTP response
        char header[256];
        sprintf(header,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %zu\r\n"
                "\r\n",
                bytes);
        // Send headers first
        write(client_fd, header, strlen(header));
        // Send the body (HTML content)
        write(client_fd, file_content, bytes);
      }
      close(client_fd);
      exit(0); // child exits after handling the request
    }

    // close the client connection
    close(client_fd);
  }
  return 0;
}
