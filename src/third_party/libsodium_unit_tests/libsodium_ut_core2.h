///
///\file libsodium_ut_core2.h
///\brief Unit tests for the sodium library core2 functions.
///
///\details
///
///\author Dmitriy Kiryanov dmitriyk@aethernet.io
///\version 1.0.0
///\date  15.07.2024
///
#ifndef THIRD_PARTY_LIBSODIUM_UT_CORE2_H
#define THIRD_PARTY_LIBSODIUM_UT_CORE2_H

#include "third_party/libsodium_unit_tests/cmptest.h"

namespace core2
{    
    #define TEST_NAME28 "core2"

    static unsigned char firstkey[32] = { 0x1b, 0x27, 0x55, 0x64, 0x73, 0xe9, 0x85,
                                          0xd4, 0x62, 0xcd, 0x51, 0x19, 0x7a, 0x9a,
                                          0x46, 0xc7, 0x60, 0x09, 0x54, 0x9e, 0xac,
                                          0x64, 0x74, 0xf2, 0x06, 0xc4, 0xee, 0x08,
                                          0x44, 0xf6, 0x83, 0x89 };

    static unsigned char nonceprefix[16] = { 0x69, 0x69, 0x6e, 0xe9, 0x55, 0xb6,
                                             0x2b, 0x73, 0xcd, 0x62, 0xbd, 0xa8,
                                             0x75, 0xfc, 0x73, 0xd6 };

    static unsigned char c[16] = { 0x65, 0x78, 0x70, 0x61, 0x6e, 0x64, 0x20, 0x33,
                                   0x32, 0x2d, 0x62, 0x79, 0x74, 0x65, 0x20, 0x6b };

    static unsigned char secondkey[32];
    
    void _libsodium_ut_core2();
} // namespace core1

#endif /* THIRD_PARTY_LIBSODIUM_UT_CORE2_H */
