#include "peer.h"

#include "tls.h"

#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int connect_to_peer(const char* host, int port, SSL_CTX* ctx, Peer* out_peer) {
  int                sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);
  inet_pton(AF_INET, host, &addr.sin_addr);

  if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    return -1;
  }

  SSL* ssl = create_tls_connection(ctx, sock_fd);
  if (SSL_connect(ssl) <= 0) { // initiates the handshake right here. This is incredible
    close(sock_fd);
    return -2;
  }

  out_peer->socket_fd = sock_fd;
  out_peer->ssl       = ssl;
  strncpy(out_peer->ip, host, INET_ADDRSTRLEN);

  return 0;
}

Peer* accept_peer(int listen_fd, SSL_CTX* ctx) {
  int                client_fd;
  struct sockaddr_in addr;
  socklen_t          fd_len = sizeof(addr);

  if ((client_fd = accept(listen_fd, (struct sockaddr*)&addr, &fd_len)) < 0) {
    return NULL;
  };

  SSL* ssl = create_tls_connection(ctx, client_fd);
  if (SSL_accept(ssl) <= 0) {
    SSL_free(ssl);
    close(client_fd);
    return NULL;
  }

  Peer* peer      = malloc(sizeof(Peer));
  peer->socket_fd = client_fd;
  peer->ssl       = ssl;
  inet_ntop(AF_INET, &addr.sin_addr, peer->ip, sizeof(peer->ip));
  return peer;
}