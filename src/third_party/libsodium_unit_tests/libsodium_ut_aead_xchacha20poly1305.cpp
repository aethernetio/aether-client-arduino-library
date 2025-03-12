#include "third_party/libsodium_unit_tests/libsodium_ut_aead_chacha20poly1305.h"

#include "unity.h"
//#include "aether/tele/tele.h"

namespace aead_xchacha20poly1305
{
    #define DEBUG_OUT 1
    #define LOG(TAG, ...) {\
     fprintf(stderr, "file location%s\nfunction name:%s\nline:%d\n", __FILE__, __FUNCTION__, __LINE__);\
     fprintf(stderr, "date:%s\ntime:%s\n",__DATE__, __TIME__);\
     fprintf(stderr, "%s", TAG);\
     fprintf(stderr, "\r\n");\
     fprintf(stderr, __VA_ARGS__);\
     fprintf(stderr, "\n");\
    }

    static const char *TAG = "SODIUM UNITY";

    #if DEBUG_OUT==1    
        #define DebugPrint(format, ...) LOG(TAG, format, ##__VA_ARGS__)
    #elif DEBUG_OUT==2
        #define DebugPrint(format, ...) AE_TELED_DEBUG(TAG, format, ##__VA_ARGS__)
    #elif DEBUG_OUT==3
        #define DebugPrint(format, ...) ESP_LOGI(TAG, format, ##__VA_ARGS__)
    #endif

    static int tv(void)
    {
    #undef  MLEN
    #define MLEN 114U
    #undef  ADLEN
    #define ADLEN 12U
    #undef  CLEN
    #define CLEN (MLEN + crypto_aead_xchacha20poly1305_ietf_ABYTES)
        static const unsigned char firstkey[crypto_aead_xchacha20poly1305_ietf_KEYBYTES]
            = {
                0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
                0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
                0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
            };
    #undef  MESSAGE
    #define MESSAGE "Ladies and Gentlemen of the class of '99: If I could offer you " \
        "only one tip for the future, sunscreen would be it."
        unsigned char *m = (unsigned char *) sodium_malloc(MLEN);
        static const unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES]
            = { 0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53 };
        static const unsigned char ad[ADLEN]
            = { 0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 };
        unsigned char *c = (unsigned char *) sodium_malloc(CLEN);
        unsigned char *detached_c = (unsigned char *) sodium_malloc(MLEN);
        unsigned char *key2 = (unsigned char *) sodium_malloc(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
        unsigned char *mac = (unsigned char *) sodium_malloc(crypto_aead_xchacha20poly1305_ietf_ABYTES);
        unsigned char *m2 = (unsigned char *) sodium_malloc(MLEN);
        unsigned long long found_clen;
        unsigned long long found_maclen;
        unsigned long long m2len;
        size_t i;

        TEST_ASSERT(sizeof MESSAGE - 1U == MLEN);
        memcpy(m, MESSAGE, MLEN);
        crypto_aead_xchacha20poly1305_ietf_encrypt(c, &found_clen, m, MLEN,
                                                   ad, ADLEN,
                                                   NULL, nonce, firstkey);
        if (found_clen != MLEN + crypto_aead_xchacha20poly1305_ietf_abytes()) {
            DebugPrint("found_clen is not properly set");
        }
        for (i = 0U; i < CLEN; ++i) {
            DebugPrint("%u", (unsigned int) c[i]);
            if (i % 8 == 7) {
                DebugPrint("\n");
            }
        }
        DebugPrint("\n");
        crypto_aead_xchacha20poly1305_ietf_encrypt_detached(detached_c,
                                                            mac, &found_maclen,
                                                            m, MLEN,
                                                            ad, ADLEN,
                                                            NULL, nonce, firstkey);
        if (found_maclen != crypto_aead_xchacha20poly1305_ietf_abytes()) {
            DebugPrint("found_maclen is not properly set");
        }
        if (memcmp(detached_c, c, MLEN) != 0) {
            DebugPrint("detached ciphertext is bogus");
        }

        if (crypto_aead_xchacha20poly1305_ietf_decrypt(NULL, 0, NULL, c, CLEN, ad,
                                                       ADLEN, nonce, firstkey) != 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() tag-only verification failed");
        }
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, CLEN, ad,
                                                       ADLEN, nonce, firstkey) != 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() failed");
        }
        if (m2len != MLEN) {
            DebugPrint("m2len is not properly set");
        }
        if (memcmp(m, m2, MLEN) != 0) {
            DebugPrint("m != m2");
        }
        memset(m2, 0, m2len);
        if (crypto_aead_xchacha20poly1305_ietf_decrypt_detached(m2, NULL,
                                                                c, MLEN, mac,
                                                                ad, ADLEN,
                                                                nonce, firstkey) != 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt_detached() failed");
        }
        if (memcmp(m, m2, MLEN) != 0) {
            DebugPrint("detached m != m2");
        }

        for (i = 0U; i < CLEN; i++) {
            c[i] ^= (i + 1U);
            if (crypto_aead_xchacha20poly1305_ietf_decrypt(m2, NULL, NULL, c, CLEN,
                                                           ad, ADLEN, nonce, firstkey)
                == 0 || memcmp(m, m2, MLEN) == 0) {
                DebugPrint("message can be forged");
            }
            c[i] ^= (i + 1U);
        }
        crypto_aead_xchacha20poly1305_ietf_encrypt(c, &found_clen, m, MLEN,
                                                   NULL, 0U, NULL, nonce, firstkey);
        if (found_clen != CLEN) {
            DebugPrint("clen is not properly set (adlen=0)");
        }
        for (i = 0U; i < CLEN; ++i) {
            DebugPrint("%u", (unsigned int) c[i]);
            if (i % 8 == 7) {
                DebugPrint("\n");
            }
        }
        DebugPrint("\n");
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, CLEN,
                                                       NULL, 0U, nonce, firstkey) != 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() failed (adlen=0)");
        }
        if (m2len != MLEN) {
            DebugPrint("m2len is not properly set (adlen=0)");
        }
        if (memcmp(m, m2, MLEN) != 0) {
            DebugPrint("m != m2 (adlen=0)");
        }
        m2len = 1;
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(
                m2, &m2len, NULL, guard_page,
                randombytes_uniform(crypto_aead_xchacha20poly1305_ietf_ABYTES),
                NULL, 0U, nonce, firstkey) != -1) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() worked with a short ciphertext");
        }
        if (m2len != 0) {
            DebugPrint("Message length should have been set to zero after a failure");
        }
        m2len = 1;
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(m2, &m2len, NULL, c, 0U, NULL, 0U,
                                                      nonce, firstkey) != -1) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() worked with an empty ciphertext");
        }
        if (m2len != 0) {
            DebugPrint("Message length should have been set to zero after a failure");
        }

        memcpy(c, m, MLEN);
        crypto_aead_xchacha20poly1305_ietf_encrypt(c, &found_clen, c, MLEN,
                                                   NULL, 0U, NULL, nonce, firstkey);
        if (found_clen != CLEN) {
            DebugPrint("clen is not properly set (adlen=0)");
        }
        for (i = 0U; i < CLEN; ++i) {
            DebugPrint("%u", (unsigned int) c[i]);
            if (i % 8 == 7) {
                DebugPrint("\n");
            }
        }
        DebugPrint("\n");

        if (crypto_aead_xchacha20poly1305_ietf_decrypt(c, &m2len, NULL, c, CLEN,
                                                       NULL, 0U, nonce, firstkey) != 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() failed (adlen=0)");
        }
        if (m2len != MLEN) {
            DebugPrint("m2len is not properly set (adlen=0)");
        }
        if (memcmp(m, c, MLEN) != 0) {
            DebugPrint("m != c (adlen=0)");
        }

        crypto_aead_xchacha20poly1305_ietf_keygen(key2);
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(c, &m2len, NULL, c, CLEN,
                                                       NULL, 0U, nonce, key2) == 0) {
            DebugPrint("crypto_aead_xchacha20poly1305_ietf_decrypt() with a wrong key should have failed");
        }

        sodium_free(c);
        sodium_free(detached_c);
        sodium_free(key2);
        sodium_free(mac);
        sodium_free(m2);
        sodium_free(m);

        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_abytes() == crypto_aead_xchacha20poly1305_ietf_ABYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_keybytes() == crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_npubbytes() == crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_nsecbytes() == 0U);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_nsecbytes() == crypto_aead_xchacha20poly1305_ietf_NSECBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_ietf_messagebytes_max() == crypto_aead_xchacha20poly1305_ietf_MESSAGEBYTES_MAX);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_IETF_KEYBYTES  == crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_IETF_NSECBYTES == crypto_aead_xchacha20poly1305_ietf_NSECBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_IETF_NPUBBYTES == crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_IETF_ABYTES    == crypto_aead_xchacha20poly1305_ietf_ABYTES);
        TEST_ASSERT(crypto_aead_xchacha20poly1305_IETF_MESSAGEBYTES_MAX == crypto_aead_xchacha20poly1305_ietf_MESSAGEBYTES_MAX);

        return 0;
    }

    ///
    ///\brief _libsodium_unit_aead_aegis256 Unit test for libsodium.
    ///\param[in] void
    ///\return void
    ///
    void _libsodium_ut_aead_xchacha20poly1305()
    {
        tv();
    }
} // namespace aead_xchacha20poly1305
