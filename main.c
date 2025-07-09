#include "io.h"
#include "tls.h"

#include <netinet/in.h>
#include <openssl/crypto.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int create_listen_socket(int port) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  addr.sin_port        = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_family      = AF_INET;

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("Failed to bind socket");
    exit(EXIT_FAILURE);
  };

  listen(sockfd, 5);
  return sockfd;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Correct use: %s <port>\n", argv[0]);
    return 1;
  }

  int      port = atoi(argv[1]);
  SSL_CTX* ctx  = create_tls_ctx();
  configure_ctx(ctx);

  int listen_fd = create_listen_socket(port);

  run_event_loop(ctx, listen_fd);
}