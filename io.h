#pragma once

#include <openssl/crypto.h>

void handle_input();
void handle_peer_data(int peer_idx);
void run_event_loop(SSL_CTX* ctx, int listen_fd);