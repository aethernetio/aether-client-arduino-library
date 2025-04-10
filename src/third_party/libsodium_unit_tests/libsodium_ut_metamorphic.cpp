#include "third_party/libsodium_unit_tests/libsodium_ut_metamorphic.h"

#include "unity.h"
//#include "aether/tele/tele.h"

namespace metamorphic
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
    
    static void
    mm_generichash(void)
    {
        crypto_generichash_state st;
        unsigned char *h, *h2;
        unsigned char *k;
        unsigned char *m;
        size_t         hlen;
        size_t         klen;
        size_t         mlen;
        size_t         l1, l2;
        int            i;

        for (i = 0; i < MAX_ITER; i++) {
            mlen = randombytes_uniform(MAXLEN);
            m = (unsigned char *) sodium_malloc(mlen);
            klen = randombytes_uniform(crypto_generichash_KEYBYTES_MAX -
                                       crypto_generichash_KEYBYTES_MIN + 1U)
                + crypto_generichash_KEYBYTES_MIN;
            k = (unsigned char *) sodium_malloc(klen);
            hlen = randombytes_uniform(crypto_generichash_BYTES_MAX -
                                       crypto_generichash_BYTES_MIN + 1U)
                + crypto_generichash_BYTES_MIN;
            h = (unsigned char *) sodium_malloc(hlen);
            h2 = (unsigned char *) sodium_malloc(hlen);

            randombytes_buf(k, klen);
            randombytes_buf(m, mlen);

            crypto_generichash_init(&st, k, klen, hlen);
            l1 = randombytes_uniform((uint32_t) mlen);
            l2 = randombytes_uniform((uint32_t) (mlen - l1));
            crypto_generichash_update(&st, m, l1);
            crypto_generichash_update(&st, m + l1, l2);
            crypto_generichash_update(&st, m + l1 + l2, mlen - l1 - l2);
            crypto_generichash_final(&st, h, hlen);

            crypto_generichash(h2, hlen, m, mlen, k, klen);

            TEST_ASSERT(memcmp(h, h2, hlen) == 0);

            sodium_free(h2);
            sodium_free(h);
            sodium_free(k);
            sodium_free(m);
        }
    }

    static void
    mm_onetimeauth(void)
    {
        crypto_onetimeauth_state st;
        unsigned char *h, *h2;
        unsigned char *k;
        unsigned char *m;
        size_t         mlen;
        size_t         l1, l2;
        int            i;

        for (i = 0; i < MAX_ITER; i++) {
            mlen = randombytes_uniform(MAXLEN);
            m = (unsigned char *) sodium_malloc(mlen);
            k = (unsigned char *) sodium_malloc(crypto_onetimeauth_KEYBYTES);
            h = (unsigned char *) sodium_malloc(crypto_onetimeauth_BYTES);
            h2 = (unsigned char *) sodium_malloc(crypto_onetimeauth_BYTES);

            crypto_onetimeauth_keygen(k);
            randombytes_buf(m, mlen);

            crypto_onetimeauth_init(&st, k);
            l1 = randombytes_uniform((uint32_t) mlen);
            l2 = randombytes_uniform((uint32_t) (mlen - l1));
            crypto_onetimeauth_update(&st, m, l1);
            crypto_onetimeauth_update(&st, m + l1, l2);
            crypto_onetimeauth_update(&st, m + l1 + l2, mlen - l1 - l2);
            crypto_onetimeauth_final(&st, h);

            crypto_onetimeauth(h2, m, mlen, k);

            TEST_ASSERT(memcmp(h, h2, crypto_onetimeauth_BYTES) == 0);

            sodium_free(h2);
            sodium_free(h);
            sodium_free(k);
            sodium_free(m);
        }
    }

    static void
    mm_hmacsha256(void)
    {
        crypto_auth_hmacsha256_state st;
        unsigned char *h, *h2;
        unsigned char *k;
        unsigned char *m;
        size_t         mlen;
        size_t         l1, l2;
        int            i;

        for (i = 0; i < MAX_ITER; i++) {
            mlen = randombytes_uniform(MAXLEN);
            m = (unsigned char *) sodium_malloc(mlen);
            k = (unsigned char *) sodium_malloc(crypto_auth_hmacsha256_KEYBYTES);
            h = (unsigned char *) sodium_malloc(crypto_auth_hmacsha256_BYTES);
            h2 = (unsigned char *) sodium_malloc(crypto_auth_hmacsha256_BYTES);

            crypto_auth_hmacsha256_keygen(k);
            randombytes_buf(m, mlen);

            crypto_auth_hmacsha256_init(&st, k, crypto_auth_hmacsha256_KEYBYTES);
            l1 = randombytes_uniform((uint32_t) mlen);
            l2 = randombytes_uniform((uint32_t) (mlen - l1));
            crypto_auth_hmacsha256_update(&st, m, l1);
            crypto_auth_hmacsha256_update(&st, m + l1, l2);
            crypto_auth_hmacsha256_update(&st, m + l1 + l2, mlen - l1 - l2);
            crypto_auth_hmacsha256_final(&st, h);

            crypto_auth_hmacsha256(h2, m, mlen, k);

            TEST_ASSERT(memcmp(h, h2, crypto_auth_hmacsha256_BYTES) == 0);

            sodium_free(h2);
            sodium_free(h);
            sodium_free(k);
            sodium_free(m);
        }
    }

    static void
    mm_hmacsha512(void)
    {
        crypto_auth_hmacsha512_state st;
        unsigned char *h, *h2;
        unsigned char *k;
        unsigned char *m;
        size_t         mlen;
        size_t         l1, l2;
        int            i;

        for (i = 0; i < MAX_ITER; i++) {
            mlen = randombytes_uniform(MAXLEN);
            m = (unsigned char *) sodium_malloc(mlen);
            k = (unsigned char *) sodium_malloc(crypto_auth_hmacsha512_KEYBYTES);
            h = (unsigned char *) sodium_malloc(crypto_auth_hmacsha512_BYTES);
            h2 = (unsigned char *) sodium_malloc(crypto_auth_hmacsha512_BYTES);

            crypto_auth_hmacsha512_keygen(k);
            randombytes_buf(m, mlen);

            crypto_auth_hmacsha512_init(&st, k, crypto_auth_hmacsha512_KEYBYTES);
            l1 = randombytes_uniform((uint32_t) mlen);
            l2 = randombytes_uniform((uint32_t) (mlen - l1));
            crypto_auth_hmacsha512_update(&st, m, l1);
            crypto_auth_hmacsha512_update(&st, m + l1, l2);
            crypto_auth_hmacsha512_update(&st, m + l1 + l2, mlen - l1 - l2);
            crypto_auth_hmacsha512_final(&st, h);

            crypto_auth_hmacsha512(h2, m, mlen, k);

            TEST_ASSERT(memcmp(h, h2, crypto_auth_hmacsha512_BYTES) == 0);

            sodium_free(h2);
            sodium_free(h);
            sodium_free(k);
            sodium_free(m);
        }
    }

    int
    test(void)
    {
        mm_generichash();
        mm_onetimeauth();
        mm_hmacsha256();
        mm_hmacsha512();

        DebugPrint("OK\n");

        return 0;
    }

    void _libsodium_ut_metamorphic()
    {
        test();
    }
} // namespace