#ifndef blake2b_long_H
#define blake2b_long_H

#include <stddef.h>
#include "third_party/libsodium/src/libsodium/include/sodium/private/quirks.h"

int blake2b_long(void *pout, size_t outlen, const void *in, size_t inlen);

#endif
