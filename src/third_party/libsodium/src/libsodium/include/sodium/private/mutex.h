#ifndef mutex_H
#define mutex_H 1

#include "third_party/libsodium/src/libsodium/include/sodium/private/quirks.h"

extern int sodium_crit_enter(void);
extern int sodium_crit_leave(void);

#endif
