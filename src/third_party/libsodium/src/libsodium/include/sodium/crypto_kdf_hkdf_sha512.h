#ifndef crypto_kdf_hkdf_sha512_H
#define crypto_kdf_hkdf_sha512_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "third_party/libsodium/src/libsodium/include/sodium/crypto_kdf.h"
#include "third_party/libsodium/src/libsodium/include/sodium/crypto_auth_hmacsha512.h"
#include "third_party/libsodium/src/libsodium/include/sodium/export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

#define crypto_kdf_hkdf_sha512_KEYBYTES crypto_auth_hmacsha512_BYTES
SODIUM_EXPORT
size_t crypto_kdf_hkdf_sha512_keybytes(void);

#define crypto_kdf_hkdf_sha512_BYTES_MIN 0U
SODIUM_EXPORT
size_t crypto_kdf_hkdf_sha512_bytes_min(void);

#define crypto_kdf_hkdf_sha512_BYTES_MAX (0xff * crypto_auth_hmacsha512_BYTES)
SODIUM_EXPORT
size_t crypto_kdf_hkdf_sha512_bytes_max(void);

SODIUM_EXPORT
int crypto_kdf_hkdf_sha512_extract(unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES],
                                   const unsigned char *salt, size_t salt_len,
                                   const unsigned char *ikm, size_t ikm_len)
            __attribute__ ((nonnull(1)));

SODIUM_EXPORT
void crypto_kdf_hkdf_sha512_keygen(unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kdf_hkdf_sha512_expand(unsigned char *out, size_t out_len,
                                  const char *ctx, size_t ctx_len,
                                  const unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
            __attribute__ ((nonnull(1)));

/* ------------------------------------------------------------------------- */

typedef struct crypto_kdf_hkdf_sha512_state {
    crypto_auth_hmacsha512_state st;
} crypto_kdf_hkdf_sha512_state;

SODIUM_EXPORT
size_t crypto_kdf_hkdf_sha512_statebytes(void);

SODIUM_EXPORT
int crypto_kdf_hkdf_sha512_extract_init(crypto_kdf_hkdf_sha512_state *state,
                                        const unsigned char *salt, size_t salt_len)
            __attribute__ ((nonnull(1)));

SODIUM_EXPORT
int crypto_kdf_hkdf_sha512_extract_update(crypto_kdf_hkdf_sha512_state *state,
                                          const unsigned char *ikm, size_t ikm_len)
            __attribute__ ((nonnull));

SODIUM_EXPORT
int crypto_kdf_hkdf_sha512_extract_final(crypto_kdf_hkdf_sha512_state *state,
                                         unsigned char prk[crypto_kdf_hkdf_sha512_KEYBYTES])
            __attribute__ ((nonnull));

#ifdef __cplusplus
}
#endif

#endif
