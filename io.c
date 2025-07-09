#include "io.h"

#include "peer.h"

#include <sys/select.h>

#define __stdin 0
#define MAX_PEERS 10

static Peer* peers[MAX_PEERS] = {0};

void handle_input() {}

void handle_peer_data(int peer_idx) {}

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

    if (FD_ISSET(__stdin, &readfds))
      handle_input();

    if (FD_ISSET(listen_fd, &readfds)) {
      Peer* p = accept_peer(listen_fd, ctx);
      for (int i = 0; i < MAX_PEERS; i++) {
        if (!peers[i]) {
          peers[i] = p;
          break;
        }
      }
    }

    for (int i = 0; i < MAX_PEERS; i++) {
      if (peers[i] && FD_ISSET(peers[i]->socket_fd, &readfds)) {
        handle_peer_data(i);
      }
    }
  }
}