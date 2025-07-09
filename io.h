#pragma once

#include <openssl/crypto.h>

void handle_input();
void run_event_loop(SSL_CTX* ctx, int listen_fd);