#ifndef poly1305_sse2_H
#define poly1305_sse2_H

#include <stddef.h>

#include "third_party/libsodium/src/libsodium/crypto_onetimeauth/poly1305/onetimeauth_poly1305.h"
#include "third_party/libsodium/src/libsodium/include/sodium/crypto_onetimeauth_poly1305.h"

extern struct crypto_onetimeauth_poly1305_implementation
    crypto_onetimeauth_poly1305_sse2_implementation;

#endif /* poly1305_sse2_H */
