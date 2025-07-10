#include "io.h"
#include "peer.h"
#include "tls.h"

#include <netinet/in.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
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

  if (argc == 2) {
    int port      = atoi(argv[1]);
    int listen_fd = create_listen_socket(port);
    printf("Waiting connections on port %d...\n", port);
    SSL_CTX* ctx = create_tls_ctx();
    configure_ctx(ctx);
    run_event_loop(ctx, listen_fd);
    fprintf(stderr, "Loop principal foi encerrado. Isso não deveria acontecer.\n");
  } else if (argc == 4 && strcmp(argv[1], "connect") == 0) {
    printf("Client mode initiated\n");
    const char* host = argv[2];
    int         port = atoi(argv[3]);
    Peer        p;
    SSL_CTX* ctx = create_tls_ctx();
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    if (connect_to_peer(host, port, ctx, &p) == 0) {
      printf("Connected to %s:%d\n", host, port);
      char msg[512];
      while (fgets(msg, sizeof(msg), stdin)) {
        SSL_write(p.ssl, msg, strlen(msg));
      }
    } else {
      ERR_print_errors_fp(stderr);
      fprintf(stderr, "Failed to connect.\n");
    }
  } else {
    fprintf(stderr, "Use:\n");
    fprintf(stderr, "  %s <port>              # server\n", argv[0]);
    fprintf(stderr, "  %s connect <ip> <porta> # client\n", argv[0]);
    return 1;
  }

  printf("failed\n");
  return 0;
}