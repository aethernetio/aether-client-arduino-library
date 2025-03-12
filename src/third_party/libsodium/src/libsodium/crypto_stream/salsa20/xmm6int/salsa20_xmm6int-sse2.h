
#include <stdint.h>

#include "third_party/libsodium/src/libsodium/crypto_stream/salsa20/stream_salsa20.h"
#include "third_party/libsodium/src/libsodium/include/sodium/crypto_stream_salsa20.h"

extern struct crypto_stream_salsa20_implementation
    crypto_stream_salsa20_xmm6int_sse2_implementation;
