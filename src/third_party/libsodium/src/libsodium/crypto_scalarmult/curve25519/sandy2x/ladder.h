#ifndef ladder_H
#define ladder_H

#ifdef __cplusplus
extern "C" {
#endif

#include "third_party/libsodium/src/libsodium/crypto_scalarmult/curve25519/sandy2x/fe.h"
#include "third_party/libsodium/src/libsodium/crypto_scalarmult/curve25519/sandy2x/ladder_namespace.h"

extern void ladder(fe *, const unsigned char *);

#ifdef __cplusplus
}
#endif

#endif /* ifndef ladder_H */

