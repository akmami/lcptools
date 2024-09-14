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
 *   - Functions to initialize hash tables (`str_map` and `core_map`) with preallocated sizes.
 *   - A function to hash a sequence of bytes from a string iterator range, using a seed value for initialization.
 * 
 * Example usage:
 * @code
 *   lcp::init_hashing(); // Initializes hash tables with default sizes.
 *   
 *   std::string sequence = "ACGT";
 *   size_t hash_value = lcp::hash_bytes(sequence.begin(), sequence.end());
 * @endcode
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

#include <unordered_map>
#include <string>
#include <iterator>
#include <cstddef>
#include <cstring>
#include "constant.h"


struct hashing {
    std::size_t operator() ( const std::array<uint, 3>& arr ) const {
        const size_t m = 0x5bd1e995;
        size_t seed = 0x153ac45c;
        std::size_t hash = seed ^ 3;
        size_t chars;
        // hash first element
        chars = arr[0];
        chars *= m;
        chars ^= chars >> 24;
        chars *= m;
        hash *= m;
        hash ^= chars;
        // hash second element
        chars = arr[1];
        chars *= m;
        chars ^= chars >> 24;
        chars *= m;
        hash *= m;
        hash ^= chars;
        // hash third element
        chars = arr[2];
        chars *= m;
        chars ^= chars >> 24;
        chars *= m;
        hash *= m;
        hash ^= chars;

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= m;
        hash ^= hash >> 15;
        return hash;
    }
};


struct equality {
    bool operator() ( const std::array<uint, 3>& lhs, const std::array<uint, 3>& rhs ) const {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
    }
};


namespace lcp {

    extern std::unordered_map<std::string, uint> str_map;
    extern std::unordered_map<std::array<uint, 3>, uint, hashing, equality> core_map;
    // extern std::unordered_map<unsigned int, std::vector<unsigned int>*> core_map_reverse;
    extern uint next_id;

    /**
     * @brief Initializes the string and core hash tables with the specified sizes.
     *
     * This function reserves memory for the `str_map` and `core_map` hash tables, optimizing for the 
     * expected number of elements to prevent hash collisions and improve lookup performance.
     *
     * @param str_map_size The initial size of the string hash table (`str_map`). Default is `1000`.
     * @param core_map_size The initial size of the core hash table (`core_map`). Default is `10000`.
     */
    void init_hashing(size_t str_map_size = STR_HASH_TABLE_SIZE, size_t core_map_size = CORE_HASH_TABLE_SIZE);

    /**
     * @brief Computes a hash value for a sequence of bytes.
     *
     * This function hashes a sequence of bytes from the specified range, using a mixing algorithm to 
     * ensure uniform distribution of hash values. It converts all characters to uppercase during the 
     * process to make the hashing case-insensitive.
     *
     * @param begin An iterator to the start of the byte sequence.
     * @param end An iterator to the end of the byte sequence.
     * @param seed An optional seed value for initializing the hash. Default is `0x153ac45c`.
     * @return The computed hash value.
     *
     */
    size_t hash_bytes(std::string::iterator begin, std::string::iterator end, size_t seed=0x153ac45c);
}

#endif