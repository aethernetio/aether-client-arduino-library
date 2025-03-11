///
///\file libsodium_ut_keygen.h
///\brief Unit tests for the sodium library keygen functions.
///
///\details
///
///\author Dmitriy Kiryanov dmitriyk@aethernet.io
///\version 1.0.0
///\date  15.07.2024
///
#ifndef THIRD_PARTY_LIBSODIUM_UT_KEYGEN_H
#define THIRD_PARTY_LIBSODIUM_UT_KEYGEN_H

#include "third_party/libsodium_unit_tests/cmptest.h"

namespace keygen
{    
    #define TEST_NAME41 "keygen"
#include "third_party/libsodium_unit_tests/cmptest.h"

    typedef struct KeygenTV_ {
        void (*fn)(unsigned char *k);
        size_t key_len;
    } KeygenTV;
    
    void _libsodium_ut_keygen();
} // namespace keygen

#endif /* THIRD_PARTY_LIBSODIUM_UT_KEYGEN_H */
