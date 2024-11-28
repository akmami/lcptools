/**
 * @file hash.h
 * @brief Defines hashing and equality structures for custom hashing of arrays, as well as 
 *        functions for managing hash tables in the LCP parsing system.
 *
 * This file provides the implementation of a custom hash function for arrays of three unsigned integers 
 * and an equality operator to be used in unordered maps. It also includes functions to initialize the hash 
 * maps for strings and cores, and to calculate hash values for byte sequences.
 *
 * Key functionalities include:
 *   - A specialized hash function for arrays of three unsigned integers, using a mixing algorithm 
 *     for robust hash generation.
 *   - An equality operator to compare arrays of three unsigned integers.
 *   - Functions to initialize hash tables (`str_map` and `cores_map`) with preallocated sizes.
 *   - A function to hash a sequence of bytes from a string iterator range, using a seed value for initialization.
 * 
 * ----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public domain.
 * The author hereby disclaims copyright to this source code.
 *
 * Note: This is a 32-bit version of MurmurHash3, adapted from the original
 * implementation. The original MurmurHash3 includes x86 and x64 versions
 * optimized for their respective platforms, which do not produce the same
 * results.
 * 
 * You can find the original MurmurHash3 code here:
 * https://github.com/aappleby/smhasher
 * ----------------------------------------------------------------------------
 * 
 * This version has been adapted for use as a consistent hashing function with 
 * a fixed seed to ensure reproducibility across different runs and platforms.
 * ----------------------------------------------------------------------------
 * 
 * @see constant.h
 * 
 * @namespace lcp
 * 
 * @author Akmuhammet Ashyralyyev
 * @version 1.0
 * @date 2024-09-14
 * 
 */

#ifndef HASH_H
#define HASH_H


#include <cstdint>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include "constant.h"


#define BIG_CONSTANT(x) (x)

namespace lcp {

    // struct core
    struct cores {
        uint32_t data[DCT_ITERATION_COUNT + 4];
        uint32_t label;

        /**
        * @brief Parameterized constructor for the `cores` structure.
        * 
        * @param label Label associated with the structure.
        * @param labels The array of subcore labels. Must have exactly DCT_ITERATION_COUNT + 3 elements.
        * 
        * Initializes the structure with the provided core values and label.
        */
        cores(const uint32_t label, const uint32_t labels[DCT_ITERATION_COUNT + 4] ) {
            this->label = label;
            std::copy_n(labels, DCT_ITERATION_COUNT + 4, data);
        }
    };

    namespace hash {
        
        // mutex
        extern std::mutex str_map_mutex;
        extern std::mutex cores_map_mutex;

        // maps
        extern std::unordered_map<std::string, uint32_t> str_map;
        extern std::vector<std::list<struct cores>> cores_map;

        // id
        extern uint32_t next_id;

        /**
         * @brief Initializes the internal hash maps with the specified sizes.
         *
         * Reserves memory for the `str_map` and `cores_map` hash maps. 
         * If `cores_map` already contains elements, a warning message is printed, 
         * and no reservation is performed for `cores_map`.
         *
         * @param str_map_size The number of elements to reserve for the `str_map`.
         * @param cores_map_size The number of elements to reserve for the `cores_map`.
         */
        void init( size_t str_map_size = STR_HASH_TABLE_SIZE, size_t cores_map_size = CORE_HASH_TABLE_SIZE );

        /**
         * @brief Inserts a string into the `str_map` and returns its unique ID.
         *
         * Converts the input string to uppercase and checks if it already exists in the `str_map`. 
         * If it exists, returns the existing ID. Otherwise, safely inserts the string into the map 
         * while ensuring thread safety and assigns it a new ID.
         *
         * @param data Pointer to the string data to be inserted.
         * @param length Length of the string. (Unused in the function)
         * @return The unique ID of the string in the `str_map`.
         */
        uint32_t emplace( const char *data, const size_t length );

        /**
         * @brief Inserts a core represented as an array of `uint32_t` into the `cores_map` and returns its unique ID.
         *
         * Computes a hash index for the core and checks if an equivalent core exists in the same bucket.
         * If it exists, returns the existing label. Otherwise, safely inserts the new core into the bucket 
         * while ensuring thread safety and assigns it a new label.
         *
         * @param data Pointer to the array representing the core.
         * @param length Length of the array.
         * @return The unique ID (label) of the core in the `cores_map`.
         */
        uint32_t emplace( const uint32_t *data, const size_t length );
        
        /**
         * @brief Computes a hash value for a given string.
         *
         * Uses the MurmurHash3 hashing algorithm to compute a hash value for the input string.
         * Frees the memory allocated for the input string after hashing.
         *
         * @param data Pointer to the string data to be hashed.
         * @param length Length of the string.
         * @return The computed hash value.
         */
        uint32_t simple( const char *data, const size_t length );

        /**
         * @brief Computes a hash value for a given array of `uint32_t`.
         *
         * Uses the MurmurHash3 hashing algorithm to compute a hash value for the input array.
         * Frees the memory allocated for the input array after hashing.
         *
         * @param data Pointer to the array to be hashed.
         * @param length Length of the array.
         * @return The computed hash value.
         */
        uint32_t simple( const uint32_t *data, const size_t length );

        /**
         * @brief Provides a summary of hash map statistics for two maps: `str_map` and `cores_map`.
         *
         * This function computes and outputs various statistics about two hash maps (`str_map` and `cores_map`),
         * including the load factor, bucket count, number of collisions, number of empty buckets, and the maximum bucket size.
         * The summary provides insight into the efficiency of the hash maps and their underlying bucket distribution.
         * 
         * The statistics are printed in the following format for each map:
         * 
         *     map_name = <load_factor> <bucket_count> <collisions> <empty_buckets> <max_bucket_size>
         * 
         * Where:
         * - `load_factor` is the ratio of the number of elements to the number of buckets.
         * - `bucket_count` is the total number of buckets.
         * - `collisions` refers to the number of hash collisions (entries sharing the same bucket).
         * - `empty_buckets` is the number of buckets with no entries.
         * - `max_bucket_size` is the largest number of entries in any single bucket.
         */
        void summary();

        // MurmurHash2-64A function

        /**
         * @brief Computes the 64-bit MurmurHash64A hash for a given key.
         * 
         * This function computes a 64-bit hash of the input data 'key' with the specified
         * length 'len' and an optional seed value. It processes the input in blocks and handles
         * any remaining bytes.
         * 
         * @param key Pointer to the data to be hashed.
         * @param len The length of the data in bytes.
         * @param seed An initial seed value for the hash computation.
         * @return The resulting 64-bit hash value.
         */
        inline uint64_t MurmurHash64A( const void * key, int len, uint64_t seed );

        // MurmurHash3 32-bit function
        
        /**
         * @brief Performs a left rotation on a 32-bit integer.
         * 
         * This function rotates the bits of the input integer 'x' to the left by 'r' positions.
         * 
         * @param x The 32-bit integer to be rotated.
         * @param r The number of positions to rotate.
         * @return The result of the left rotation.
         */
        inline uint32_t rotl32( uint32_t x, int8_t r );

        /**
         * @brief Finalizes the hash value using mix functions.
         * 
         * This function applies several mixing operations to the hash value 'h' to ensure
         * a uniform distribution of the final hash.
         * 
         * @param h The hash value to be mixed.
         * @return The final mixed hash value.
         */
        inline uint32_t fmix32( uint32_t h );

        /**
         * @brief Computes the 32-bit MurmurHash3 hash for a given key.
         * 
         * This function computes a 32-bit hash of the input data 'key' with the specified
         * length 'len' and an optional seed value. It processes the input in blocks and handles
         * any remaining bytes.
         * 
         * @param key Pointer to the data to be hashed.
         * @param len The length of the data in bytes.
         * @param seed An initial seed value for the hash computation.
         * @return The resulting 32-bit hash value.
         */
        inline uint32_t MurmurHash3_32( const void* key, int len, uint32_t seed = 42 );
    };
};

#endif