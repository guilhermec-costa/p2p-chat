#include "io.h"

#include "peer.h"

#include <openssl/ssl.h>
#include <sys/select.h>
#include <unistd.h>

#define __stdin 0
#define MAX_PEERS 10

static Peer* peers[MAX_PEERS] = {0};

void handle_input() {
  char msg[512];
  fgets(msg, sizeof(msg), stdin);
  for (int i = 0; i < MAX_PEERS; i++) {
    if (peers[i]) {
      SSL_write(peers[i]->ssl, msg, sizeof(msg));
    }
  }
}

void handle_peer_data(int peer_idx) {
  char buf[512];
  int  n = SSL_read(peers[peer_idx]->ssl, buf, sizeof(buf) - 1);

  if (n <= 0) {
    printf("Peer %s disconnected.\n", peers[peer_idx]->ip);
    SSL_shutdown(peers[peer_idx]->ssl);
    SSL_free(peers[peer_idx]->ssl);
    close(peers[peer_idx]->socket_fd);
    free(peers[peer_idx]);
    peers[peer_idx] = NULL;
    return;
  }

  buf[n] = '\0';
  printf("<%s>: %s", peers[peer_idx]->ip, buf);
}

void run_event_loop(SSL_CTX* ctx, int listen_fd) {
  fd_set readfds;
  int    max_fd;

  while (1) {
    FD_ZERO(&readfds);
    FD_SET(__stdin, &readfds);
    FD_SET(listen_fd, &readfds);
    max_fd = listen_fd;

    for (int i = 0; i < MAX_PEERS; i++) {
      if (peers[i]) {
        FD_SET(peers[i]->socket_fd, &readfds);
        if (peers[i]->socket_fd > max_fd)
          max_fd = peers[i]->socket_fd;
      }
    }

    if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0)
      continue;

    if (FD_ISSET(__stdin, &readfds)) {
      printf("Handling input from stdin\n");
      handle_input();
    }

    if (FD_ISSET(listen_fd, &readfds)) {
      Peer* p = accept_peer(listen_fd, ctx);
      if(p) {
        printf("Received connection from remote peer. IP: %s\n", p->ip);
        for (int i = 0; i < MAX_PEERS; i++) {
          if (!peers[i]) {
            peers[i] = p;
            break;
          }
        }
      }
    }

    for (int i = 0; i < MAX_PEERS; i++) {
      if (peers[i] && FD_ISSET(peers[i]->socket_fd, &readfds)) {
        printf("Handling peer reading\n");
        handle_peer_data(i);
      }
    }
  }
}