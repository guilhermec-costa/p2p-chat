#include "tls.h"

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

SSL_CTX* create_tls_ctx() {
  SSL_CTX* ctx;
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  ctx = SSL_CTX_new(TLS_method());
  return ctx;
}

void configure_ctx(SSL_CTX* ctx) {
  if(
    SSL_CTX_use_certificate_file(ctx, "./certs/cert.pem", SSL_FILETYPE_PEM) <= 0 ||
    SSL_CTX_use_PrivateKey_file(ctx, "./certs/key.pem", SSL_FILETYPE_PEM) <= 0
  ) {
    ERR_print_errors_fp(stderr);
    exit(1);
  }
}

SSL* create_tls_connection(SSL_CTX* ctx, int fd) {
  SSL* ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  return ssl;
}