
#include <stdint.h>

#include "third_party/libsodium/src/libsodium/crypto_stream/chacha20/stream_chacha20.h"
#include "third_party/libsodium/src/libsodium/include/sodium/crypto_stream_chacha20.h"

extern struct crypto_stream_chacha20_implementation
    crypto_stream_chacha20_dolbeau_avx2_implementation;
