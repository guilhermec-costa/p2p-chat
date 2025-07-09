#pragma once

#include <openssl/crypto.h>
#include <openssl/ssl.h>

SSL_CTX* create_tls_ctx();
void     configure_ctx(SSL_CTX*);
SSL*     create_tls_connection(SSL_CTX* ctx, int fd);