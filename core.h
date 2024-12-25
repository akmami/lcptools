/**
 * @file core.h
 * @brief Header file for the `core` struct, which represents bit-encoded
 * sequences and provides various utilities for encoding, compression, and file
 * I/O operations on bit sequences. (I/O operations will be supported later)
 *
 * This file contains the declaration of the `core` struct along with its member
 * functions, constructors, destructors, and operator overloads. It supports
 * operations for encoding strings into bit sequences, combining multiple
 * sequences, compressing data, and reading/writing from/to files.
 *
 * Key functionalities include:
 * - Encoding sequences of characters (e.g., ACGT) into a compact bit-encoded
 * form.
 * - Supporting reverse complement encoding of DNA sequences.
 * - Saving and loading core from files.
 * - Calculating memory usage of the constructed core structure.
 *
 * Dependencies:
 * - Requires constant.h and encoding.h for auxiliary data structures and
 * utilities.
 *
 * @see constant.h
 * @see encoding.h
 *
 * @namespace lcp
 * @struct core
 *
 *
 * @author Akmuhammet Ashyralyyev
 * @version 1.0
 * @date 2024-09-14
 *
 */

#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "encoding.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

#define UBLOCK_BIT_SIZE 32
#define DCT_ITERATION_COUNT 1

#define minimum(a, b) ((a) < (b) ? (a) : (b))

typedef unsigned int ublock;
typedef uint32_t ubit_size;
typedef uint32_t ulabel;

struct core {
    ubit_size bit_size;
    ublock *bit_rep;
    ulabel label;
    uint64_t start;
    uint64_t end;
};

/**
 * @brief Computes the 32-bit MurmurHash3 hash for a given key.
 *
 * This function computes a 32-bit hash of the input data 'key' with the
 * specified length 'len' and an optional seed value. It processes the
 * input in blocks and handles any remaining bytes.
 *
 * @param key Pointer to the data to be hashed.
 * @param len The length of the data in bytes.
 * @param seed An initial seed value for the hash computation.
 * @return The resulting 32-bit hash value.
 */
uint32_t MurmurHash3_32(const void *key, int len, uint32_t seed);

void init_core1(struct core *cr, const char *begin, uint64_t distance, uint64_t start_index, uint64_t end_index);

void init_core2(struct core *cr, const char *begin, uint64_t distance, uint64_t start_index, uint64_t end_index);

void init_core3(struct core *cr, struct core *begin, uint64_t distance);

void init_core4(struct core *cr, ubit_size bit_size, ublock *bit_rep, ulabel label, uint64_t start, uint64_t end);

void free_core(struct core* cr);

/**
 * @brief Compresses the right `core` object by comparing it with
 * left `core` object.
 *
 * This function compresses the core's bit sequence by identifying
 * common patterns between the right `core` object and left `core` object,
 * and reduces the size of the sequence accordingly.
 *
 * @param left_core The `core` object to compare against for compression.
 * @param right_core The `core` object that will be compressed.
 */
void core_compress(const struct core *left_core, struct core *right_core);

/**
 * @brief Calculates the total memory size used by the `core` object.
 *
 * This function computes the memory used by the `core` object,
 * including the bit sequence and metadata.
 *
 * @return The total memory size in bytes.
 */
uint64_t core_memsize(const struct core *cr);

/**
 * @brief Output the bit representation of a `core` object.
 *
 * This function iterates over the bits in the `core` object and outputs
 * them to the given output stream. It prints each bit as either 0 or 1,
 * starting from the most significant bit to the least significant bit.
 *
 * @param cr The `core` object to be output.
 */
void print_core(const struct core *cr);

// core comparison operator implementation

/**
 * @brief Operator overload for equality comparison between two `core`
 * objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the two objects are equal, 0 otherwise.
 */
int core_eq(const struct core *lhs, const struct core *rhs);

/**
 * @brief Operator overload for greater-than comparison between two `core`
 * objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the left-hand object is greater, 0 otherwise.
 */
int core_neq(const struct core *lhs, const struct core *rhs);

/**
 * @brief Operator overload for smaller-than comparison between two `core`
 * objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the left-hand object is smaller, 0 otherwise.
 */
int core_gt(const struct core *lhs, const struct core *rhs);

/**
 * @brief Operator overload for not-equal-to comparison between two `core`
 * objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the two objects are not equal, 0 otherwise.
 */
int core_lt(const struct core *lhs, const struct core *rhs);

/**
 * @brief Operator overload for greater-than-or-equal-to comparison between
 * two `core` objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the left-hand object is greater than or equal, 0 otherwise.
 */
int core_geq(const struct core *lhs, const struct core *rhs);

/**
 * @brief Operator overload for smaller-than-or-equal-to comparison between
 * two `core` objects.
 *
 * @param lhs The left-hand side `core` object.
 * @param rhs The right-hand side `core` object.
 * @return 1 if the left-hand object is smaller than or equal, 0 otherwise.
 */
int core_leq(const struct core *lhs, const struct core *rhs);

#ifdef __cplusplus
}
#endif

#endif