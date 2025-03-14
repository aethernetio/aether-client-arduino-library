
#include "third_party/libsodium/src/libsodium/include/sodium/crypto_scalarmult_curve25519.h"
#include "third_party/libsodium/src/libsodium/include/sodium/private/implementations.h"
#include "third_party/libsodium/src/libsodium/crypto_scalarmult/curve25519/scalarmult_curve25519.h"
#include "third_party/libsodium/src/libsodium/include/sodium/runtime.h"

#ifdef HAVE_AVX_ASM
#include "third_party/libsodium/src/libsodium/crypto_scalarmult/curve25519/sandy2x/curve25519_sandy2x.h"
#endif
#include "third_party/libsodium/src/libsodium/crypto_scalarmult/curve25519/ref10/x25519_ref10.h"
static const crypto_scalarmult_curve25519_implementation *implementation =
    &crypto_scalarmult_curve25519_ref10_implementation;

int
crypto_scalarmult_curve25519(unsigned char *q, const unsigned char *n,
                             const unsigned char *p)
{
    size_t                 i;
    volatile unsigned char d = 0;

    if (implementation->mult(q, n, p) != 0) {
        return -1; /* LCOV_EXCL_LINE */
    }
    for (i = 0; i < crypto_scalarmult_curve25519_BYTES; i++) {
        d |= q[i];
    }
    return -(1 & ((d - 1) >> 8));
}

int
crypto_scalarmult_curve25519_base(unsigned char *q, const unsigned char *n)
{
    return crypto_scalarmult_curve25519_ref10_implementation
        .mult_base(q, n);
}

size_t
crypto_scalarmult_curve25519_bytes(void)
{
    return crypto_scalarmult_curve25519_BYTES;
}

size_t
crypto_scalarmult_curve25519_scalarbytes(void)
{
    return crypto_scalarmult_curve25519_SCALARBYTES;
}

int
_crypto_scalarmult_curve25519_pick_best_implementation(void)
{
    implementation = &crypto_scalarmult_curve25519_ref10_implementation;

#ifdef HAVE_AVX_ASM
    if (sodium_runtime_has_avx()) {
        implementation = &crypto_scalarmult_curve25519_sandy2x_implementation;
    }
#endif
    return 0;
}
