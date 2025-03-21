#ifndef implementations_H
#define implementations_H

#include "third_party/libsodium/src/libsodium/include/sodium/private/quirks.h"

int _crypto_generichash_blake2b_pick_best_implementation(void);
int _crypto_onetimeauth_poly1305_pick_best_implementation(void);
int _crypto_pwhash_argon2_pick_best_implementation(void);
int _crypto_scalarmult_curve25519_pick_best_implementation(void);
int _crypto_stream_chacha20_pick_best_implementation(void);
int _crypto_stream_salsa20_pick_best_implementation(void);
int _crypto_aead_aegis128l_pick_best_implementation(void);
int _crypto_aead_aegis256_pick_best_implementation(void);

#endif
