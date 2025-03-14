#ifndef sign_ed25519_ref10_H
#define sign_ed25519_ref10_H

#include "third_party/libsodium/src/libsodium/include/sodium/private/quirks.h"

void _crypto_sign_ed25519_ref10_hinit(crypto_hash_sha512_state *hs,
                                      int prehashed);

int _crypto_sign_ed25519_detached(unsigned char *sig,
                                  unsigned long long *siglen_p,
                                  const unsigned char *m,
                                  unsigned long long mlen,
                                  const unsigned char *sk, int prehashed);

int _crypto_sign_ed25519_verify_detached(const unsigned char *sig,
                                         const unsigned char *m,
                                         unsigned long long   mlen,
                                         const unsigned char *pk,
                                         int prehashed);
#endif
