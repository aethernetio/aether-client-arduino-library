
#ifndef randombytes_internal_random_H
#define randombytes_internal_random_H

#include "third_party/libsodium/src/libsodium/include/sodium/export.h"
#include "third_party/libsodium/src/libsodium/include/sodium/randombytes.h"

#ifdef __cplusplus
extern "C" {
#endif

SODIUM_EXPORT
extern struct randombytes_implementation randombytes_internal_implementation;

/* Backwards compatibility with libsodium < 1.0.18 */
#define randombytes_salsa20_implementation randombytes_internal_implementation

#ifdef __cplusplus
}
#endif

#endif
