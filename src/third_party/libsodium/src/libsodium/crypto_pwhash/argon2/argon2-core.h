/*
 * Argon2 source code package
 *
 * Written by Daniel Dinu and Dmitry Khovratovich, 2015
 *
 * This work is licensed under a Creative Commons CC0 1.0 License/Waiver.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with
 * this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef argon2_core_H
#define argon2_core_H

#include <string.h>

#include "third_party/libsodium/src/libsodium/crypto_pwhash/argon2/argon2.h"
#include "third_party/libsodium/src/libsodium/include/sodium/private/quirks.h"

/*************************Argon2 internal
 * constants**************************************************/

enum argon2_ctx_constants {
    /* Version of the algorithm */
    ARGON2_VERSION_NUMBER = 0x13,

    /* Memory block size in bytes */
    ARGON2_BLOCK_SIZE      = 1024,
    ARGON2_QWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 8,
    ARGON2_OWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 16,
    ARGON2_HWORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 32,
    ARGON2_512BIT_WORDS_IN_BLOCK = ARGON2_BLOCK_SIZE / 64,

    /* Number of pseudo-random values generated by one call to Blake in Argon2i
       to
       generate reference block positions */
    ARGON2_ADDRESSES_IN_BLOCK = 128,

    /* Pre-hashing digest length and its extension*/
    ARGON2_PREHASH_DIGEST_LENGTH = 64,
    ARGON2_PREHASH_SEED_LENGTH   = 72
};

/*************************Argon2 internal data
 * types**************************************************/

/*
 * Structure for the (1KB) memory block implemented as 128 64-bit words.
 * Memory blocks can be copied, XORed. Internal words can be accessed by [] (no
 * bounds checking).
 */
typedef struct block_ {
    uint64_t v[ARGON2_QWORDS_IN_BLOCK];
} block;

typedef struct block_region_ {
    void * base;
    block *memory;
    size_t size;
} block_region;

/*****************Functions that work with the block******************/

/* Initialize each byte of the block with @in */
static inline void
init_block_value(block *b, uint8_t in)
{
    memset(b->v, in, sizeof(b->v));
}

/* Copy block @src to block @dst */
static inline void
copy_block(block *dst, const block *src)
{
    memcpy(dst->v, src->v, sizeof(uint64_t) * ARGON2_QWORDS_IN_BLOCK);
}

/* XOR @src onto @dst bytewise */
static inline void
xor_block(block *dst, const block *src)
{
    int i;
    for (i = 0; i < ARGON2_QWORDS_IN_BLOCK; ++i) {
        dst->v[i] ^= src->v[i];
    }
}

/*
 * Argon2 instance: memory pointer, number of passes, amount of memory, type,
 * and derived values.
 * Used to evaluate the number and location of blocks to construct in each
 * thread
 */
typedef struct Argon2_instance_t {
    block_region *region;        /* Memory region pointer */
    uint64_t     *pseudo_rands;
    uint32_t      passes;        /* Number of passes */
    uint32_t      current_pass;
    uint32_t      memory_blocks; /* Number of blocks in memory */
    uint32_t      segment_length;
    uint32_t      lane_length;
    uint32_t      lanes;
    uint32_t      threads;
    argon2_type   type;
    int           print_internals; /* whether to print the memory blocks */
} argon2_instance_t;

/*
 * Argon2 position: where we construct the block right now. Used to distribute
 * work between threads.
 */
typedef struct Argon2_position_t {
    uint32_t pass;
    uint32_t lane;
    uint8_t  slice;
    uint32_t index;
} argon2_position_t;

/*Struct that holds the inputs for thread handling FillSegment*/
typedef struct Argon2_thread_data {
    argon2_instance_t *instance_ptr;
    argon2_position_t  pos;
} argon2_thread_data;

/*************************Argon2 core
 * functions**************************************************/

/*
 * Computes absolute position of reference block in the lane following a skewed
 * distribution and using a pseudo-random value as input
 * @param instance Pointer to the current instance
 * @param position Pointer to the current position
 * @param pseudo_rand 32-bit pseudo-random value used to determine the position
 * @param same_lane Indicates if the block will be taken from the current lane.
 * If so we can reference the current segment
 * @pre All pointers must be valid
 */
static uint32_t index_alpha(const argon2_instance_t *instance,
                            const argon2_position_t *position, uint32_t pseudo_rand,
                            int same_lane)
{
    /*
     * Pass 0:
     *      This lane : all already finished segments plus already constructed
     * blocks in this segment
     *      Other lanes : all already finished segments
     * Pass 1+:
     *      This lane : (SYNC_POINTS - 1) last segments plus already constructed
     * blocks in this segment
     *      Other lanes : (SYNC_POINTS - 1) last segments
     */
    uint32_t reference_area_size;
    uint64_t relative_position, absolute_position;
    uint32_t start_position;

    if (position->pass == 0) {
        /* First pass */
        if (position->slice == 0) {
            /* First slice */
            reference_area_size =
                position->index - 1; /* all but the previous */
        } else {
            if (same_lane) {
                /* The same lane => add current segment */
                reference_area_size =
                    position->slice * instance->segment_length +
                    position->index - 1;
            } else {
                reference_area_size =
                    position->slice * instance->segment_length +
                    ((position->index == 0) ? (-1) : 0);
            }
        }
    } else {
        /* Second pass */
        if (same_lane) {
            reference_area_size = instance->lane_length -
                                  instance->segment_length + position->index -
                                  1;
        } else {
            reference_area_size = instance->lane_length -
                                  instance->segment_length +
                                  ((position->index == 0) ? (-1) : 0);
        }
    }

    /* 1.2.4. Mapping pseudo_rand to 0..<reference_area_size-1> and produce
     * relative position */
    relative_position = pseudo_rand;
    relative_position = relative_position * relative_position >> 32;
    relative_position = reference_area_size - 1 -
                        (reference_area_size * relative_position >> 32);

    /* 1.2.5 Computing starting position */
    start_position = 0;

    if (position->pass != 0) {
        start_position = (position->slice == ARGON2_SYNC_POINTS - 1)
                             ? 0
                             : (position->slice + 1) * instance->segment_length;
    }

    /* 1.2.6. Computing absolute position */
    absolute_position = start_position + relative_position - instance->lane_length;
    absolute_position += instance->lane_length & (absolute_position >> 32);
    return (uint32_t) absolute_position;
}

/*
 * Function that validates all inputs against predefined restrictions and return
 * an error code
 * @param context Pointer to current Argon2 context
 * @return ARGON2_OK if everything is all right, otherwise one of error codes
 * (all defined in <argon2.h>
 */
int argon2_validate_inputs(const argon2_context *context);

/*
 * Function allocates memory, hashes the inputs with Blake,  and creates first
 * two blocks. Returns the pointer to the main memory with 2 blocks per lane
 * initialized
 * @param  context  Pointer to the Argon2 internal structure containing memory
 * pointer, and parameters for time and space requirements.
 * @param  instance Current Argon2 instance
 * @return Zero if successful, -1 if memory failed to allocate. @context->state
 * will be modified if successful.
 */
int argon2_initialize(argon2_instance_t *instance, argon2_context *context);

/*
 * XORing the last block of each lane, hashing it, making the tag. Deallocates
 * the memory.
 * @param context Pointer to current Argon2 context (use only the out parameters
 * from it)
 * @param instance Pointer to current instance of Argon2
 * @pre instance->state must point to necessary amount of memory
 * @pre context->out must point to outlen bytes of memory
 * @pre if context->free_cbk is not NULL, it should point to a function that
 * deallocates memory
 */
void argon2_finalize(const argon2_context *context,
                     argon2_instance_t *instance);

/*
 * Function that fills the segment using previous segments also from other
 * threads
 * @param instance Pointer to the current instance
 * @param position Current position
 * @pre all block pointers must be valid
 */
typedef void (*fill_segment_fn)(const argon2_instance_t *instance,
                                argon2_position_t        position);
void argon2_fill_segment_avx512f(const argon2_instance_t *instance,
                                 argon2_position_t        position);
void argon2_fill_segment_avx2(const argon2_instance_t *instance,
                              argon2_position_t        position);
void argon2_fill_segment_ssse3(const argon2_instance_t *instance,
                               argon2_position_t        position);
void argon2_fill_segment_ref(const argon2_instance_t *instance,
                             argon2_position_t        position);

/*
 * Function that fills the entire memory t_cost times based on the first two
 * blocks in each lane
 * @param instance Pointer to the current instance
 * @return Zero if successful, -1 if memory failed to allocate
 */
void argon2_fill_memory_blocks(argon2_instance_t *instance, uint32_t pass);

#endif
