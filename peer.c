#include "peer.h"
#include "tls.h"
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

Peer* accept_peer(int listen_fd, SSL_CTX* ctx) {
  int client_fd;
  struct sockaddr_in addr;
  socklen_t fd_len = sizeof(addr);

  if((client_fd = accept(listen_fd, (struct sockaddr*)&addr, &fd_len)) < 0) {
    return NULL;
  };

  SSL* ssl = create_tls_connection(ctx, client_fd);
  if(SSL_accept(ssl) <= 0) {
    SSL_free(ssl);
    close(client_fd);
    return NULL;
  }

  Peer* peer = malloc(sizeof(Peer));
  peer->socket_fd = client_fd;
  peer->ssl = ssl;
  inet_ntop(AF_INET, &addr.sin_addr, peer->ip, sizeof(peer->ip));
  return peer;
}