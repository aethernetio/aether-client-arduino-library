#include "third_party/libsodium/src/libsodium/include/sodium/crypto_shorthash_siphash24.h"

size_t
crypto_shorthash_siphashx24_bytes(void) {
    return crypto_shorthash_siphashx24_BYTES;
}

size_t
crypto_shorthash_siphashx24_keybytes(void) {
    return crypto_shorthash_siphashx24_KEYBYTES;
}
