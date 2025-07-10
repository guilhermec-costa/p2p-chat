#include <netinet/in.h>
#include <openssl/ssl.h>

typedef struct {
  int  socket_fd;
  SSL* ssl;
  char ip[INET_ADDRSTRLEN];
} Peer;

Peer* accept_peer(int listen_fd, SSL_CTX* ctx);
int   connect_to_peer(const char* host, int port, SSL_CTX* ctx, Peer* out_peer);